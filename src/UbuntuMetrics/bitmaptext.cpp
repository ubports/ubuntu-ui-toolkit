// Copyright © 2016 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Ubuntu UI Toolkit.
//
// Ubuntu UI Toolkit is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; version 3.
//
// Ubuntu UI Toolkit is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ubuntu UI Toolkit. If not, see <http://www.gnu.org/licenses/>.

#include <math.h>
#include "bitmaptext_p.h"
#include "bitmaptextfont_p.h"
#include "ubuntumetricsglobal_p.h"
#include <QtCore/QtGlobal>
#include <QtCore/QSize>
#include <QtCore/QPoint>

static const GLchar* bitmapTextVertexShaderSource =
#if !defined(QT_OPENGL_ES_2)
    "#define highp \n"
    "#define mediump \n"
    "#define lowp \n"
#endif
    "attribute highp vec4 positionAttrib; \n"
    "attribute mediump vec2 textureCoordAttrib; \n"
    "varying mediump vec2 textureCoord; \n"
    "uniform highp vec4 transform; \n"
    "void main(void) \n"
    "{ \n"
    "    gl_Position = vec4((positionAttrib.xy * transform.xy) + transform.zw, 0.0, 1.0); \n"
    "    textureCoord = textureCoordAttrib; \n"
    "} \n";

static const GLchar* bitmapTextFragmentShaderSource =
#if !defined(QT_OPENGL_ES_2)
    "#define highp \n"
    "#define mediump \n"
    "#define lowp \n"
#endif
    "varying mediump vec2 textureCoord; \n"
    "uniform sampler2D texture; \n"
    "uniform lowp float opacity; \n"
    "void main() \n"
    "{ \n"
    "    gl_FragColor = texture2D(texture, textureCoord) * vec4(opacity); \n"
    "} \n";

const int bitmapTextDefaultFontSize = 16;
const float bitmapTextDefaultOpacity = 1.0f;
const float bitmapTextCarriageReturnHeight = 1.5f;

BitmapText::BitmapText()
    : m_functions(nullptr)
#if !defined QT_NO_DEBUG
    , m_context(nullptr)
#endif
    , m_vertexBuffer(nullptr)
    , m_textToVertexBuffer(nullptr)
    , m_textLength(0)
    , m_characterCount(0)
    , m_flags(0)
{
    // Set current font based on requested font size.
    const int fontSize = qBound(
        static_cast<int>(g_bitmapTextFont.font[0].size), bitmapTextDefaultFontSize & (INT_MAX - 1),
        static_cast<int>(g_bitmapTextFont.font[g_bitmapTextFont.fontCount-1].size));
    for (int i = 0; i < g_bitmapTextFont.fontCount; i++) {
        if (static_cast<int>(g_bitmapTextFont.font[i].size) == fontSize) {
            m_currentFont = i;
            break;
        }
    }
}

BitmapText::~BitmapText()
{
    delete [] m_vertexBuffer;
    delete [] m_textToVertexBuffer;
}

static GLuint createProgram(QOpenGLFunctions* functions, const char* vertexShaderSource,
                            const char* fragmentShaderSource, GLuint* vertexShaderObject,
                            GLuint* fragmentShaderObject)
{
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint status;

    vertexShader = functions->glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = functions->glCreateShader(GL_FRAGMENT_SHADER);
    if (vertexShader == 0 || fragmentShader == 0) {
        WARN("ApplicationMonitor: glCreateShader() failed (OpenGL error: %d).", glGetError());
        return 0;
    }

    functions->glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    functions->glCompileShader(vertexShader);
    functions->glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
#if !defined(QT_NO_DEBUG)
        char infoLog[2048];
        functions->glGetShaderInfoLog(vertexShader, 2048, nullptr, infoLog);
        WARN("ApplicationMonitor: Vertex shader compilation failed:\n%s", infoLog);
#endif
        return 0;
    }

    functions->glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    functions->glCompileShader(fragmentShader);
    functions->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
#if !defined(QT_NO_DEBUG)
        char infoLog[2048];
        functions->glGetShaderInfoLog(fragmentShader, 2048, nullptr, infoLog);
        WARN("ApplicationMonitor: Fragment shader compilation failed:\n%s", infoLog);
#endif
        return 0;
    }

    program = functions->glCreateProgram();
    functions->glAttachShader(program, vertexShader);
    functions->glAttachShader(program, fragmentShader);
    functions->glLinkProgram(program);
    functions->glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
#if !defined(QT_NO_DEBUG)
        char infoLog[2048];
        functions->glGetProgramInfoLog(program, 2048, nullptr, infoLog);
        WARN("ApplicationMonitor: Shader linking failed:\n%s", infoLog);
#endif
        return 0;
    }

    *vertexShaderObject = vertexShader;
    *fragmentShaderObject = fragmentShader;

    return program;
}

bool BitmapText::initialize()
{
    DASSERT(!(m_flags & Initialized));
    DASSERT(QOpenGLContext::currentContext());

    m_functions = QOpenGLContext::currentContext()->functions();
#if !defined QT_NO_DEBUG
    m_context = QOpenGLContext::currentContext();
#endif

    m_functions->glGenTextures(1, &m_texture);
    m_functions->glBindTexture(GL_TEXTURE_2D, m_texture);
    m_functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_bitmapTextFont.textureWidth,
                              g_bitmapTextFont.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                              g_bitmapTextFont.textureData);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_program = createProgram(
        m_functions, bitmapTextVertexShaderSource, bitmapTextFragmentShaderSource,
        &m_vertexShaderObject, &m_fragmentShaderObject);
    if (m_program != 0) {
        m_functions->glBindAttribLocation(m_program, 0, "positionAttrib");
        m_functions->glBindAttribLocation(m_program, 1, "textureCoordAttrib");
        m_functions->glUniform1i(m_functions->glGetUniformLocation(m_program, "texture"), 0);
        m_programTransform = m_functions->glGetUniformLocation(m_program, "transform");
        m_programOpacity = m_functions->glGetUniformLocation(m_program, "opacity");
        m_functions->glUniform1f(m_programOpacity, bitmapTextDefaultOpacity);
    }

    m_functions->glGenBuffers(1, &m_indexBuffer);

    if (m_texture && m_program && m_indexBuffer) {
#if !defined QT_NO_DEBUG
        m_flags |= Initialized;
#endif
        return true;
    } else {
        return false;
    }
}

void BitmapText::finalize()
{
    DASSERT(m_flags & Initialized);
    DASSERT(m_context == QOpenGLContext::currentContext());

    if (m_texture) {
        m_functions->glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }

    if (m_program) {
        m_functions->glDeleteProgram(m_program);
        m_functions->glDeleteShader(m_vertexShaderObject);
        m_functions->glDeleteShader(m_fragmentShaderObject);
        m_program = 0;
        m_vertexShaderObject = 0;
        m_fragmentShaderObject = 0;
    }

    if (m_indexBuffer) {
        m_functions->glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
    }

    m_functions = nullptr;
#if !defined QT_NO_DEBUG
    m_context = nullptr;
    m_flags &= ~Initialized;
#endif
}

void BitmapText::setText(const char* text)
{
    DASSERT(m_flags & Initialized);

    int textLength = 0;
    int characterCount = 0;

    // Count the number of printable characters and get the given text length.
    if (text) {
        while (text[textLength] != '\0') {
            const char character = text[textLength++];
            if (character >= 32 && character <= 126) {  // Printable characters.
                characterCount++;
            }
        }
    }

    // Clean up and update info.
    if (m_characterCount) {
        delete [] m_vertexBuffer;
        delete [] m_textToVertexBuffer;
    }
    if (characterCount) {
        m_textLength = textLength;
        m_characterCount = characterCount;
        m_flags |= NotEmpty;
    } else {
        // Early exit if the given text is null, empty or filled with non
        // printable characters.
        m_vertexBuffer = nullptr;
        m_textToVertexBuffer = nullptr;
        m_textLength = 0;
        m_characterCount = 0;
        m_flags &= ~NotEmpty;
        return;
    }

    // Fill the index buffer. The GL_TRIANGLES primitive mode requires 3 indices
    // per triangle, so 6 per character.
    GLushort* indices = new GLushort [6 * characterCount];
    for (int i = 0; i < characterCount; i++) {
        const GLushort currentIndex = i * 6;
        const GLushort currentVertex = i * 4;
        indices[currentIndex] = currentVertex;
        indices[currentIndex+1] = currentVertex + 1;
        indices[currentIndex+2] = currentVertex + 2;
        indices[currentIndex+3] = currentVertex + 2;
        indices[currentIndex+4] = currentVertex + 1;
        indices[currentIndex+5] = currentVertex + 3;
    }
    m_functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    m_functions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * characterCount * sizeof(GLushort),
                              indices, GL_STATIC_DRAW);  // Deletes and replaces the old data.
    delete [] indices;

    // Allocate and fill the vertex buffer and the text to vertex buffer array.
    m_vertexBuffer = new Vertex [characterCount * 4];
    m_textToVertexBuffer = new int [textLength];
    const float fontY = static_cast<float>(g_bitmapTextFont.font[m_currentFont].y);
    const float fontWidth = static_cast<float>(g_bitmapTextFont.font[m_currentFont].width);
    const float fontHeight = static_cast<float>(g_bitmapTextFont.font[m_currentFont].height);
    const float fontWidthNormalized = fontWidth / g_bitmapTextFont.textureWidth;
    const float fontHeightNormalized = fontHeight / g_bitmapTextFont.textureHeight;
    const float t1 = fontY / g_bitmapTextFont.textureHeight;
    const float t2 = (fontHeight + fontY) / g_bitmapTextFont.textureHeight;
    float x = 0.0f;
    float y = 0.0f;
    characterCount = 0;
    for (int i = 0; i < textLength; i++) {
        char character = text[i];
        if (character >= ' ' && character <= '~') {  // Printable characters.
            const int index = characterCount * 4;
            // The atlas stores 2 lines per font size, second line starts at
            // ASCII character 80 at position 49 in the bitmap.
            const float s = ((character - ' ') % '0') * fontWidthNormalized;
            const float t = (character < 80) ? t1 : t2;
            m_vertexBuffer[index].x = x;
            m_vertexBuffer[index].y = y;
            m_vertexBuffer[index].s = s;
            m_vertexBuffer[index].t = t;
            m_vertexBuffer[index+1].x = x;
            m_vertexBuffer[index+1].y = y + 1.0f;
            m_vertexBuffer[index+1].s = s;
            m_vertexBuffer[index+1].t = t + fontHeightNormalized;
            m_vertexBuffer[index+2].x = x + 1.0f;
            m_vertexBuffer[index+2].y = y;
            m_vertexBuffer[index+2].s = s + fontWidthNormalized;
            m_vertexBuffer[index+2].t = t;
            m_vertexBuffer[index+3].x = x + 1.0f;
            m_vertexBuffer[index+3].y = y + 1.0f;
            m_vertexBuffer[index+3].s = s + fontWidthNormalized;
            m_vertexBuffer[index+3].t = t + fontHeightNormalized;
            x += 1.0f;
            m_textToVertexBuffer[i] = characterCount++;
        } else if (character == '\n') {
            x = 0.0f;
            y += 1.0f;
            m_textToVertexBuffer[i] = -1;
        } else if (character == '\r') {
            x = 0.0f;
            y += bitmapTextCarriageReturnHeight;
            m_textToVertexBuffer[i] = -1;
        } else {
            m_textToVertexBuffer[i] = -1;
        }
    }
}

void BitmapText::updateText(const char* text, int index, int length)
{
    DASSERT(m_flags & Initialized);
    DASSERT(text);
    DASSERT(index >= 0 && index <= m_textLength);
    DASSERT(index + length <= m_textLength);

    const float fontY = static_cast<float>(g_bitmapTextFont.font[m_currentFont].y);
    const float fontWidth = static_cast<float>(g_bitmapTextFont.font[m_currentFont].width);
    const float fontHeight = static_cast<float>(g_bitmapTextFont.font[m_currentFont].height);
    const float fontWidthNormalized = fontWidth / g_bitmapTextFont.textureWidth;
    const float fontHeightNormalized = fontHeight / g_bitmapTextFont.textureHeight;
    const float t1 = fontY / g_bitmapTextFont.textureHeight;
    const float t2 = (fontHeight + fontY) / g_bitmapTextFont.textureHeight;

    for (int i = index, j = 0; i < index + length; i++, j++) {
        int vertexBufferIndex = m_textToVertexBuffer[i];
        const char character = text[j];
        if (vertexBufferIndex != -1 && character >= 32 && character <= 126) {
            const float s = ((character - ' ') % '0') * fontWidthNormalized;
            const float t = (character < 80) ? t1 : t2;
            vertexBufferIndex *= 4;
            m_vertexBuffer[vertexBufferIndex].s = s;
            m_vertexBuffer[vertexBufferIndex].t = t;
            m_vertexBuffer[vertexBufferIndex+1].s = s;
            m_vertexBuffer[vertexBufferIndex+1].t = t + fontHeightNormalized;
            m_vertexBuffer[vertexBufferIndex+2].s = s + fontWidthNormalized;
            m_vertexBuffer[vertexBufferIndex+2].t = t;
            m_vertexBuffer[vertexBufferIndex+3].s = s + fontWidthNormalized;
            m_vertexBuffer[vertexBufferIndex+3].t = t + fontHeightNormalized;
        }
    }
}

void BitmapText::bindProgram()
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_flags & Initialized);

    m_functions->glUseProgram(m_program);
}

void BitmapText::setTransform(const QSize& viewportSize, const QPointF& position)
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_flags & Initialized);
    DASSERT(viewportSize.width() > 0.0f);
    DASSERT(viewportSize.height() > 0.0f);
    DASSERT(!qIsNaN(position.x()));
    DASSERT(!qIsNaN(position.y()));

    // The transform stores a scale (in (x, y)) and translate (in (z, w)) used
    // to put vertices in the right space ((-1, 1), (-1, 1)), at the right
    // position.
    const float transform[4] = {
         (2.0f * g_bitmapTextFont.font[m_currentFont].width)  / viewportSize.width(),
        -(2.0f * g_bitmapTextFont.font[m_currentFont].height) / viewportSize.height(),
        ((2.0f *  roundf(position.x())) / viewportSize.width())  - 1.0f,
        ((2.0f * -roundf(position.y())) / viewportSize.height()) + 1.0f
    };
    m_functions->glUniform4fv(m_programTransform, 1, transform);
}

void BitmapText::setOpacity(float opacity)
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_flags & Initialized);
    DASSERT(opacity >= 0.0f && opacity <= 1.0f);

    m_functions->glUniform1f(m_programOpacity, opacity);
}

void BitmapText::render()
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_flags & Initialized);

    if (m_flags & NotEmpty) {
        m_functions->glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<char*>(m_vertexBuffer));
        m_functions->glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            reinterpret_cast<char*>(m_vertexBuffer) + (2 * sizeof(float)));
        m_functions->glEnableVertexAttribArray(0);
        m_functions->glEnableVertexAttribArray(1);
        m_functions->glBindTexture(GL_TEXTURE_2D, m_texture);
        m_functions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
        m_functions->glDisable(GL_DEPTH_TEST);  // QtQuick renderers restore that at each draw call.
        m_functions->glEnable(GL_BLEND);
        m_functions->glDrawElements(GL_TRIANGLES, 6 * m_characterCount, GL_UNSIGNED_SHORT, 0);
    }
}
