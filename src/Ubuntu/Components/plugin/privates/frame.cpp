/*
 * Copyright 2016 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

#include "frame.h"
#include "textures.h"
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>

const QRgb defaultColor = qRgba(255, 255, 255, 255);
const float defaultThickness = 20.0f;
const float defaultRadius = 50.0f;

// --- Shader ---

class FrameShader : public QSGMaterialShader
{
public:
    FrameShader();
    virtual char const* const* attributeNames() const;
    virtual void initialize();
    virtual void updateState(
        const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect);

private:
    int m_matrixId;
    int m_opacityId;
};

FrameShader::FrameShader()
{
    setShaderSourceFile(
        QOpenGLShader::Vertex, QStringLiteral(":/uc/privates/shaders/frame.vert"));
    setShaderSourceFile(
        QOpenGLShader::Fragment, QStringLiteral(":/uc/privates/shaders/frame.frag"));
}

char const* const* FrameShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "outerCoordAttrib", "innerCoordAttrib", "colorAttrib", 0
    };
    return attributes;
}

void FrameShader::initialize()
{
    QSGMaterialShader::initialize();
    program()->bind();
    program()->setUniformValue("texture", 0);
    m_matrixId = program()->uniformLocation("matrix");
    m_opacityId = program()->uniformLocation("opacity");
}

void FrameShader::updateState(
    const RenderState& state, QSGMaterial* newEffect, QSGMaterial* oldEffect)
{
    Q_UNUSED(oldEffect);

    QOpenGLFunctions* funcs = QOpenGLContext::currentContext()->functions();
    funcs->glBindTexture(
        GL_TEXTURE_2D, static_cast<UCFrameMaterial*>(newEffect)->textureId());

    if (state.isMatrixDirty()) {
        program()->setUniformValue(m_matrixId, state.combinedMatrix());
    }
    if (state.isOpacityDirty()) {
        program()->setUniformValue(m_opacityId, state.opacity());
    }
}

// --- Material ---

const int maxTextures = 16;
static struct { QOpenGLContext* openglContext; quint32 textureId; } textures[maxTextures];

// Gets the textures' slot used by the given context, or -1 if not stored.
static int getTexturesIndex(const QOpenGLContext* openglContext)
{
    int index = 0;
    while (textures[index].openglContext != openglContext) {
        index++;
        if (index == maxTextures) {
            return -1;
        }
    }
    return index;
}

// Gets an empty textures' slot.
static int getEmptyTexturesIndex()
{
    int index = 0;
    while (textures[index].openglContext) {
        index++;
        if (index == maxTextures) {
            // Don't bother with a dynamic array, let's just set a high enough
            // maxTextures and increase the static array size if ever needed.
            qFatal("reached maximum number of OpenGL contexts supported per item.");
        }
    }
    return index;
}

UCFrameMaterial::UCFrameMaterial()
{
    setFlag(Blending, true);

    // Get the texture stored per context and shared by all materials of the same type.
    QOpenGLContext* openglContext = QOpenGLContext::currentContext();
    int index = getTexturesIndex(openglContext);
    if (index < 0) {
        QOpenGLFunctions* funcs = openglContext->functions();
        index = getEmptyTexturesIndex();
        textures[index].openglContext = openglContext;
        funcs->glGenTextures(1, &textures[index].textureId);
        funcs->glBindTexture(GL_TEXTURE_2D, textures[index].textureId);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // FIXME(loicm) GL_LINEAR_MIPMAP_[NEAREST,LINEAR] perf/quality tradeoff.
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // Set the highest mipmap level in order to avoid clamp to edge issues
        // with inner corners starting from mipmap level 5 (OpenGL ES 2 doesn't
        // support GL_TEXTURE_MAX_LOD).
        funcs->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 4.0f);
        for (int i = 0; i < shapeMipmapCount; i++) {
            funcs->glTexImage2D(GL_TEXTURE_2D, i, GL_LUMINANCE, shapeMipmapBaseSize >> i,
                                shapeMipmapBaseSize >> i, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                                &shapeMipmapData[shapeMipmapOffsets[i]]);
        }
        QObject::connect(
            openglContext, &QOpenGLContext::aboutToBeDestroyed, [index] {
                QOpenGLFunctions* funcs = textures[index].openglContext->functions();
                funcs->glDeleteTextures(1, &textures[index].textureId);
                textures[index].openglContext = NULL;
            });
    }
    m_textureId = textures[index].textureId;
}

QSGMaterialType* UCFrameMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* UCFrameMaterial::createShader() const
{
    return new FrameShader;
}

int UCFrameMaterial::compare(const QSGMaterial* other) const
{
    Q_UNUSED(other);
    return 0;
}

// --- Node ---

UCFrameNode::UCFrameNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), 20, 34, GL_UNSIGNED_SHORT)
{
    memcpy(m_geometry.indexData(), indices(), 34 * sizeof(unsigned short));
    m_geometry.setDrawingMode(GL_TRIANGLE_STRIP);
    m_geometry.setIndexDataPattern(QSGGeometry::StaticPattern);
    m_geometry.setVertexDataPattern(QSGGeometry::AlwaysUploadPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
    qsgnode_set_description(this, QLatin1String("frame"));
}

// static
const unsigned short* UCFrameNode::indices()
{
    // The geometry is made of 20 vertices indexed with a triangle strip mode.
    //     0 -1 ----- 2- 3
    //     |   4 --- 5   |
    //     6  /       \  9
    //     | 7         8 |
    //     | |         | |
    //     | 11       12 |
    //    10  \       /  13
    //     |   14---15   |
    //    16 -17 --- 18- 19
    static const unsigned short indices[] = {
        6, 7, 0, 4, 1, 5, 2,
        2, 2,  // Degenerate triangle.
        2, 5, 3, 8, 9, 12, 13,
        13, 13,  // Degenerate triangle.
        13, 12, 19, 15, 18, 14, 17,
        17, 17,  // Degenerate triangle.
        17, 14, 16, 11, 10, 7, 6
    };
    return indices;
}

// static
const QSGGeometry::AttributeSet& UCFrameNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(3, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        4, sizeof(Vertex), attributes
    };
    return attributeSet;
}

// Pack a color in a premultiplied 32-bit ABGR value.
static quint32 packColor(QRgb color)
{
    const quint32 a = qAlpha(color);
    const quint32 b = ((qBlue(color) * a) + 0xff) >> 8;
    const quint32 g = ((qGreen(color) * a) + 0xff) >> 8;
    const quint32 r = ((qRed(color) * a) + 0xff) >> 8;
    return (a << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);
}

void UCFrameNode::updateGeometry(
    const QSizeF& itemSize, float thickness, float radius, QRgb color)
{
    UCFrameNode::Vertex* v = reinterpret_cast<UCFrameNode::Vertex*>(m_geometry.vertexData());
    const float w = static_cast<float>(itemSize.width());
    const float h = static_cast<float>(itemSize.height());
    const float maxSize = qMin(w, h) * 0.5f;
    const float clampedThickness = qMin(thickness, maxSize);
    const float radiusOut = qBound(0.01f, radius, maxSize);
    const float radiusIn = radiusOut * ((maxSize - clampedThickness) / maxSize);
    const float outerCoord1 =
        (((1.0f - shapeOffset) / radiusOut) * (clampedThickness + radiusIn)) + shapeOffset;
    const float outerCoord2 =
        (((1.0f - shapeOffset) / radiusOut) * clampedThickness) + shapeOffset;
    const float innerCoord1 =
        (((1.0f - shapeOffset) / radiusIn) * -clampedThickness) + shapeOffset;
    const float innerCoord2 =
        (((1.0f - shapeOffset) / radiusIn) * (radiusOut - clampedThickness)) + shapeOffset;
    const quint32 packedColor = packColor(color);

    // 1st row.
    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].s1 = shapeOffset;
    v[0].t1 = shapeOffset;
    v[0].s2 = innerCoord1;
    v[0].t2 = innerCoord1;
    v[0].color = packedColor;
    v[1].x = radiusOut;
    v[1].y = 0.0f;
    v[1].s1 = 1.0f;
    v[1].t1 = shapeOffset;
    v[1].s2 = innerCoord2;
    v[1].t2 = innerCoord1;
    v[1].color = packedColor;
    v[2].x = w - radiusOut;
    v[2].y = 0.0f;
    v[2].s1 = 1.0f;
    v[2].t1 = shapeOffset;
    v[2].s2 = innerCoord2;
    v[2].t2 = innerCoord1;
    v[2].color = packedColor;
    v[3].x = w;
    v[3].y = 0.0f;
    v[3].s1 = shapeOffset;
    v[3].t1 = shapeOffset;
    v[3].s2 = innerCoord1;
    v[3].t2 = innerCoord1;
    v[3].color = packedColor;

    // 2nd row.
    v[4].x = clampedThickness + radiusIn;
    v[4].y = clampedThickness;
    v[4].s1 = outerCoord1;
    v[4].t1 = outerCoord2;
    v[4].s2 = 1.0f;
    v[4].t2 = shapeOffset;
    v[4].color = packedColor;
    v[5].x = w - (clampedThickness + radiusIn);
    v[5].y = clampedThickness;
    v[5].s1 = outerCoord1;
    v[5].t1 = outerCoord2;
    v[5].s2 = 1.0f;
    v[5].t2 = shapeOffset;
    v[5].color = packedColor;

    // 3rd row.
    v[6].x = 0.0f;
    v[6].y = radiusOut;
    v[6].s1 = shapeOffset;
    v[6].t1 = 1.0f;
    v[6].s2 = innerCoord1;
    v[6].t2 = innerCoord2;
    v[6].color = packedColor;
    v[7].x = clampedThickness;
    v[7].y = clampedThickness + radiusIn;
    v[7].s1 = outerCoord2;
    v[7].t1 = outerCoord1;
    v[7].s2 = shapeOffset;
    v[7].t2 = 1.0f;
    v[7].color = packedColor;
    v[8].x = w - clampedThickness;
    v[8].y = clampedThickness + radiusIn;
    v[8].s1 = outerCoord2;
    v[8].t1 = outerCoord1;
    v[8].s2 = shapeOffset;
    v[8].t2 = 1.0f;
    v[8].color = packedColor;
    v[9].x = w;
    v[9].y = radiusOut;
    v[9].s1 = shapeOffset;
    v[9].t1 = 1.0f;
    v[9].s2 = innerCoord1;
    v[9].t2 = innerCoord2;
    v[9].color = packedColor;

    // 4th row.
    v[10].x = 0.0f;
    v[10].y = h - radiusOut;
    v[10].s1 = shapeOffset;
    v[10].t1 = 1.0f;
    v[10].s2 = innerCoord1;
    v[10].t2 = innerCoord2;
    v[10].color = packedColor;
    v[11].x = clampedThickness;
    v[11].y = h - (clampedThickness + radiusIn);
    v[11].s1 = outerCoord2;
    v[11].t1 = outerCoord1;
    v[11].s2 = shapeOffset;
    v[11].t2 = 1.0f;
    v[11].color = packedColor;
    v[12].x = w - clampedThickness;
    v[12].y = h - (clampedThickness + radiusIn);
    v[12].s1 = outerCoord2;
    v[12].t1 = outerCoord1;
    v[12].s2 = shapeOffset;
    v[12].t2 = 1.0f;
    v[12].color = packedColor;
    v[13].x = w;
    v[13].y = h - radiusOut;
    v[13].s1 = shapeOffset;
    v[13].t1 = 1.0f;
    v[13].s2 = innerCoord1;
    v[13].t2 = innerCoord2;
    v[13].color = packedColor;

    // 5th row.
    v[14].x = clampedThickness + radiusIn;
    v[14].y = h - clampedThickness;
    v[14].s1 = outerCoord1;
    v[14].t1 = outerCoord2;
    v[14].s2 = 1.0f;
    v[14].t2 = shapeOffset;
    v[14].color = packedColor;
    v[15].x = w - (clampedThickness + radiusIn);
    v[15].y = h - clampedThickness;
    v[15].s1 = outerCoord1;
    v[15].t1 = outerCoord2;
    v[15].s2 = 1.0f;
    v[15].t2 = shapeOffset;
    v[15].color = packedColor;

    // 6th row.
    v[16].x = 0.0f;
    v[16].y = h;
    v[16].s1 = shapeOffset;
    v[16].t1 = shapeOffset;
    v[16].s2 = innerCoord1;
    v[16].t2 = innerCoord1;
    v[16].color = packedColor;
    v[17].x = radiusOut;
    v[17].y = h;
    v[17].s1 = 1.0f;
    v[17].t1 = shapeOffset;
    v[17].s2 = innerCoord2;
    v[17].t2 = innerCoord1;
    v[17].color = packedColor;
    v[18].x = w - radiusOut;
    v[18].y = h;
    v[18].s1 = 1.0f;
    v[18].t1 = shapeOffset;
    v[18].s2 = innerCoord2;
    v[18].t2 = innerCoord1;
    v[18].color = packedColor;
    v[19].x = w;
    v[19].y = h;
    v[19].s1 = shapeOffset;
    v[19].t1 = shapeOffset;
    v[19].s2 = innerCoord1;
    v[19].t2 = innerCoord1;
    v[19].color = packedColor;

    markDirty(QSGNode::DirtyGeometry);
}

// --- Item ---

UCFrame::UCFrame(QQuickItem* parent)
    : QQuickItem(parent)
    , m_color(defaultColor)
    , m_thickness(defaultThickness)
    , m_radius(defaultRadius)
{
    setFlag(ItemHasContents);
}

void UCFrame::setThickness(qreal thickness)
{
    thickness = qMax(0.0f, static_cast<float>(thickness));
    if (m_thickness != thickness) {
        m_thickness = thickness;
        update();
        Q_EMIT thicknessChanged();
    }
}

void UCFrame::setRadius(qreal radius)
{
    radius = qMax(0.0f, static_cast<float>(radius));
    if (m_radius != radius) {
        m_radius = radius;
        update();
        Q_EMIT radiusChanged();
    }
}

void UCFrame::setColor(const QColor& color)
{
    const QRgb rgbColor = qRgba(color.red(), color.green(), color.blue(), color.alpha());
    if (m_color != rgbColor) {
        m_color = rgbColor;
        update();
        Q_EMIT colorChanged();
    }
}

QSGNode* UCFrame::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    const QSizeF itemSize(width(), height());
    if (itemSize.isEmpty() || m_thickness <= 0.0f) {
        delete oldNode;
        return NULL;
    }

    UCFrameNode* node = oldNode ? static_cast<UCFrameNode*>(oldNode) : new UCFrameNode();
    node->updateGeometry(itemSize, m_thickness, m_radius, m_color);

    return node;
}
