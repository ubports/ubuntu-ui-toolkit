/*
 * Copyright 2015 Canonical Ltd.
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

// This program creates the texture (as a C header) used by the UbuntuShape for its shape, shadows
// and bevel. It uses distance fields to create efficient anti-aliased and resolution independent
// contours. The EDTAA3 algorithm and implementation comes from Stefan Gustavson, for more
// information see http://webstaff.itn.liu.se/~stegu/aadist/readme.pdf.

// In order to generate a new file, the following commands must be used:
// $ cd tools
// $ qmake && make
// $ ./createshapetextures shape.svg ../plugin/ucubuntushapetexture.h

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
#include <math.h>  // Needed by edtaa3func.c.
#include "3rd_party/edtaa3func.c"

// Input data.
const int textureCount = 2;
const double distanceScale = 4.0;
const double shapeOffset = 0.0625;

// Sizes (must be power-of-2).
const int width = 32;
const int height = 32;
const int size = width * height;
const int widthMipmap = 256;
const int heightMipmap = 256;
const int sizeMipmap = widthMipmap * heightMipmap;
const int mipmapCount = 9;  // Minimum size is 1.
const int biggestSize = size > sizeMipmap ? size : sizeMipmap;

// Shape buffers.
static uint renderBuffer[biggestSize];
static double renderBufferNormalized[biggestSize];
// Distance field buffers.
static double distanceIn[biggestSize];
static double distanceOut[biggestSize];
// Temporary buffers used by the computegradient() and edtaa3() functions exposed by edtaa3func.c.
static short distanceX[biggestSize];
static short distanceY[biggestSize];
static double gradientX[biggestSize];
static double gradientY[biggestSize];

// Final texture buffers.
static uint textureData[size];
static uint textureDataMipmap[sizeMipmap];

// Clear rendering buffer.
static void clearRenderBuffer()
{
    memset(renderBuffer, 0, biggestSize * sizeof(uint));
}

// Clear gradient buffers.
static void clearGradientBuffers()
{
    memset(gradientX, 0, biggestSize * sizeof(double));
    memset(gradientY, 0, biggestSize * sizeof(double));
}

// Creates the inset and flat aspect texture.
static void createTexture1(
    QSvgRenderer* svg, QPainter* painter, uint* data, int width, int height, bool useEdtaa3)
{
    // Input data.
    const double shadowScale = 7.5;
    const double shadowTranslucency = 0.37;
    const double distanceBottomTY = 0.0546875;  // From shapeOffset.
    const double shadowTopTX = -0.01171875;     // From shapeOffset.
    const double shadowTopTY = 0.03125;         // From shapeOffset.
    const double shadowBottomTX = -0.01171875;  // From shapeOffset.
    const double shadowBottomTY = -0.00390625;  // From shapeOffset.

    const int size = width * height;
    const double imageScale = 255.0 / width;

    if (useEdtaa3) {
        // Render and store the distance field used for masking the top of the shape, for masking
        // the bevel and for masking the unstyled shape.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate(shapeOffset * width, shapeOffset * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = static_cast<double>(renderBuffer[i] >> 24) / 255.0;
        }
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceOut);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = 1.0 - renderBufferNormalized[i];
        }
        clearGradientBuffers();
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceIn);
        for (int i = 0; i < size; i++) {
            const double distance = qMax(0.0, distanceIn[i]) - qMax(0.0, distanceOut[i]);
            const uint value =
                qBound(0, qRound(distance * distanceScale * imageScale + 127.5), 255);
            data[i] = value << 16;  // Stored in channel R (B in shaders).
        }

        // Render and store the distance field used for masking the bottom of the shape and for
        // masking the bevel.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate(shapeOffset * width, (shapeOffset + distanceBottomTY) * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = static_cast<double>(renderBuffer[i] >> 24) / 255.0;
        }
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceOut);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = 1.0 - renderBufferNormalized[i];
        }
        clearGradientBuffers();
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceIn);
        for (int i = 0; i < size; i++) {
            const double distance = qMax(0.0, distanceIn[i]) - qMax(0.0, distanceOut[i]);
            const uint value =
                qBound(0, qRound(distance * distanceScale * imageScale + 127.5), 255);
            data[i] |= value << 24;  // Stored in channel A.
        }
    } else {
        // Render and store the mask for the top of the shape, the bevel and the unstyled shape.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate(shapeOffset * width, shapeOffset * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            const uint value = renderBuffer[i] >> 24;
            data[i] = value << 16;  // Stored in channel R (B in shaders).
        }

        // Render and store the mask for the bottom of the shape and for the bevel.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate(shapeOffset * width, (shapeOffset + distanceBottomTY) * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            const uint value = renderBuffer[i] >> 24;
            data[i] |= value << 24;  // Stored in channel A.
        }
    }

    // Render and store the top inner shadow.
    clearRenderBuffer();
    painter->resetTransform();
    painter->translate((shapeOffset + shadowTopTX) * width, (shapeOffset + shadowTopTY) * height);
    svg->render(painter);
    for (int i = 0; i < size; i++) {
        renderBufferNormalized[i] = 1.0 - (static_cast<double>(renderBuffer[i] >> 24) / 255.0);
    }
    computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
    edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        double shadow = qBound(0.0, (distanceIn[i] * shadowScale * imageScale) / 255.0, 1.0);
        shadow = (1.0 - (2.0 * shadow - shadow * shadow)) * 255.0;
        const uint value = qBound(0, qRound(shadow * shadowTranslucency), 255);
        data[i] |= value << 0;  // Stored in channel B (R in shaders).
    }

    // Render and store the bottom inner shadow.
    clearRenderBuffer();
    painter->resetTransform();
    painter->translate(
        (shapeOffset + shadowBottomTX) * width, (shapeOffset + shadowBottomTY) * height);
    svg->render(painter);
    for (int i = 0; i < size; i++) {
        renderBufferNormalized[i] = 1.0 - (static_cast<double>(renderBuffer[i] >> 24) / 255.0);
    }
    computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
    edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        double shadow = qBound(0.0, (distanceIn[i] * shadowScale * imageScale) / 255.0, 1.0);
        shadow = (1.0 - (2.0 * shadow - shadow * shadow)) * 255.0;
        const uint value = qBound(0, qRound(shadow * shadowTranslucency), 255);
        data[i] |= value << 8;  // Stored in channel G.
    }
}

// Creates the drop shadow aspect texture.
static void createTexture2(
    QSvgRenderer* svg, QPainter* painter, uint* data, int width, int height, bool useEdtaa3)
{
    // Input data.
    const double shadowScale = 4.5;
    const double shadowTranslucency = 0.8;
    const double distanceBottomTY = 0.0546875;         // From shapeOffset.
    const double distanceTx = distanceBottomTY * 0.5;  // From shapeOffset.

    const int size = width * height;
    const double imageScale = 255.0 / width;

    if (useEdtaa3) {
        // Render and store the distance field used for masking the top of the shape.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate((shapeOffset + distanceTx) * width, shapeOffset * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = static_cast<double>(renderBuffer[i] >> 24) / 255.0;
        }
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceOut);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = 1.0 - renderBufferNormalized[i];
        }
        clearGradientBuffers();
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceIn);
        for (int i = 0; i < size; i++) {
            const double distance = qMax(0.0, distanceIn[i]) - qMax(0.0, distanceOut[i]);
            const uint value =
                qBound(0, qRound(distance * distanceScale * imageScale + 127.5), 255);
            data[i] = value << 0;  // Stored in channel B (R in shaders).
        }

        // Render and store the distance field used for masking the bottom of the shape.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate(
            (shapeOffset + distanceTx) * width, (shapeOffset + distanceBottomTY) * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = static_cast<double>(renderBuffer[i] >> 24) / 255.0;
        }
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceOut);
        for (int i = 0; i < size; i++) {
            renderBufferNormalized[i] = 1.0 - renderBufferNormalized[i];
        }
        clearGradientBuffers();
        computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
        edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
               distanceIn);
        for (int i = 0; i < size; i++) {
            const double distance = qMax(0.0, distanceIn[i]) - qMax(0.0, distanceOut[i]);
            const uint value =
                qBound(0, qRound(distance * distanceScale * imageScale + 127.5), 255);
            data[i] |= value << 8;  // Stored in channel G.
        }
    } else {
        // Render and store the mask for the top of the shape.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate((shapeOffset + distanceTx) * width, shapeOffset * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            const uint value = renderBuffer[i] >> 24;
            data[i] = value << 0;  // Stored in channel B (R in shaders).
        }

        // Render and store the mask for the bottom of the shape.
        clearRenderBuffer();
        painter->resetTransform();
        painter->translate(
            (shapeOffset + distanceTx) * width, (shapeOffset + distanceBottomTY) * height);
        svg->render(painter);
        for (int i = 0; i < size; i++) {
            const uint value = renderBuffer[i] >> 24;
            data[i] |= value << 8;  // Stored in channel G.
        }
    }

    // Render and store the bottom drop shadow.
    clearRenderBuffer();
    painter->resetTransform();
    painter->translate(shapeOffset * width, shapeOffset * height);
    svg->render(painter);
    for (int i = 0; i < size; i++) {
        renderBufferNormalized[i] = 1.0 - (static_cast<double>(renderBuffer[i] >> 24) / 255.0);
    }
    computegradient(renderBufferNormalized, width, height, gradientX, gradientY);
    edtaa3(renderBufferNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        double shadow = qBound(0.0, (distanceIn[i] * shadowScale * imageScale) / 255.0, 1.0);
        shadow = (2.0 * shadow - shadow * shadow) * 255.0;
        const uint value = qBound(0, qRound(shadow * shadowTranslucency), 255);
        data[i] |= value << 16;  // Stored in channel R (B in shaders).
    }
}

static void dumpTexture(QTextStream& cppOut, const uint* data, int size)
{
    cppOut.setIntegerBase(16);
    cppOut.setFieldWidth(2);
    cppOut.setPadChar('0');
    if (size >= 16) {
        for (int i = 0; i < size; i += 4) {
            cppOut << "    \""
                   << "\\x" << (data[i] & 0xff)
                   << "\\x" << ((data[i] >> 8) & 0xff)
                   << "\\x" << ((data[i] >> 16) & 0xff)
                   << "\\x" << ((data[i] >> 24) & 0xff)
                   << "\\x" << (data[i+1] & 0xff)
                   << "\\x" << ((data[i+1] >> 8) & 0xff)
                   << "\\x" << ((data[i+1] >> 16) & 0xff)
                   << "\\x" << ((data[i+1] >> 24) & 0xff)
                   << "\\x" << (data[i+2] & 0xff)
                   << "\\x" << ((data[i+2] >> 8) & 0xff)
                   << "\\x" << ((data[i+2] >> 16) & 0xff)
                   << "\\x" << ((data[i+2] >> 24) & 0xff)
                   << "\\x" << (data[i+3] & 0xff)
                   << "\\x" << ((data[i+3] >> 8) & 0xff)
                   << "\\x" << ((data[i+3] >> 16) & 0xff)
                   << "\\x" << ((data[i+3] >> 24) & 0xff);
            cppOut.setFieldWidth(0);
            cppOut << "\"\n";
            cppOut.setFieldWidth(2);
        }
        cppOut.setIntegerBase(10);
        cppOut.setFieldWidth(0);
        cppOut.setPadChar(' ');
    } else {
        cppOut << "    \"";
        for (int i = 0; i < size; i++) {
            cppOut << "\\x" << (data[i] & 0xff)
                   << "\\x" << ((data[i] >> 8) & 0xff)
                   << "\\x" << ((data[i] >> 16) & 0xff)
                   << "\\x" << ((data[i] >> 24) & 0xff);
        }
        cppOut.setIntegerBase(10);
        cppOut.setFieldWidth(0);
        cppOut.setPadChar(' ');
        cppOut << "\"\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        qWarning("Usage: createshapetextures input_svg output_cpp");
        return 1;
    }
    const char* svgFilename = argv[1];
    const char* cppFilename = argv[2];

    // Open files.
    QSvgRenderer svg;
    if (!svg.load(QString(svgFilename))) {
        qWarning("Can't open input SVG file \'%s\'", svgFilename);
        return 1;
    }
    QFile cppFile(cppFilename);
    if (!cppFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Can't create output C++ file \'%s\'", cppFilename);
        return 1;
    }

    QTextStream cppOut(&cppFile);
    cppOut << "// Copyright 2015 Canonical Ltd.\n"
           << "// Automatically generated by the createshapetextures tool.\n\n";

    // Create the distance field textures and write them to the C++ file.
    QImage shape(reinterpret_cast<uchar*>(renderBuffer), width, height,
                 width * 4, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&shape);
    cppOut << "const int shapeTextureCount = " << textureCount << ";\n"
           << "const int shapeTextureWidth = " << width << ";\n"
           << "const int shapeTextureHeight = " << height << ";\n"
           << "const float shapeTextureOffset = " << shapeOffset << ";\n"
           << "const int shapeTextureDistanceAA = " << distanceScale << ";\n"
           << "static const unsigned char shapeTextureData[" << textureCount
           <<   "][" << width * height * 4 + 1 << "] = {\n";
    createTexture1(&svg, &painter, textureData, width, height, true);
    dumpTexture(cppOut, textureData, width * height);
    cppOut << "    ,\n";
    createTexture2(&svg, &painter, textureData, width, height, true);
    dumpTexture(cppOut, textureData, width * height);
    cppOut << "};\n\n";
    painter.end();

    // Create the mipmap textures and write them to the C++ file.
    cppOut << "const int shapeTextureMipmapWidth = " << widthMipmap << ";\n"
           << "const int shapeTextureMipmapHeight = " << heightMipmap << ";\n"
           << "const int shapeTextureMipmapCount = " << mipmapCount << ";\n"
           << "static const int shapeTextureMipmapOffset[" << mipmapCount << "] = {\n"
           << "    0";
    int size = 0, i = 0;
    for (int i = 0; i < mipmapCount-1; i++) {
        size += (widthMipmap >> i) * (heightMipmap >> i) * 4;
        cppOut << ", " << size;
    }
    size += (widthMipmap >> (mipmapCount-1)) * (heightMipmap >> (mipmapCount-1)) * 4;
    cppOut << "\n};\n";
    cppOut << "static const unsigned char shapeTextureMipmapData[" << textureCount
           <<   "][" << size + 1 << "] = {\n";
    for (int i = 0; i < mipmapCount; i++) {
        const int width = widthMipmap >> i;
        const int height = heightMipmap >> i;
        QImage shapeMipmap(reinterpret_cast<uchar*>(renderBuffer), width, height,
                           width * 4, QImage::Format_ARGB32_Premultiplied);
        painter.begin(&shapeMipmap);
        createTexture1(&svg, &painter, textureDataMipmap, width, height, false);
        cppOut << "    // Mipmap level " << i << ".\n";
        dumpTexture(cppOut, textureDataMipmap, width * height);
        painter.end();
    }
    cppOut << "    ,\n";
    for (int i = 0; i < mipmapCount; i++) {
        const int width = widthMipmap >> i;
        const int height = heightMipmap >> i;
        QImage shapeMipmap(reinterpret_cast<uchar*>(renderBuffer), width, height,
                           width * 4, QImage::Format_ARGB32_Premultiplied);
        painter.begin(&shapeMipmap);
        createTexture2(&svg, &painter, textureDataMipmap, width, height, false);
        cppOut << "    // Mipmap level " << i << ".\n";
        dumpTexture(cppOut, textureDataMipmap, width * height);
        painter.end();
    }
    cppOut << "};\n";

    return 0;
}
