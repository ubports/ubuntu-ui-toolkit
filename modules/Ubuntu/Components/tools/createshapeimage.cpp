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
// $ ./createshapeimage shape.svg ../plugin/ucubuntushapetexture.h

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
#include <math.h>  // Needed by edtaa3func.c.
#include "3rd_party/edtaa3func.c"

// Input data.
const int textureSize = 32;  // Must be a power-of-2.
const double distanceScale = 4.0;
const double shadowScale = 7.5;
const double shadowTranslucency = 0.37;
const double shapeOffset = 0.0625;
const double distanceBottomTY = 0.0546875;  // From shapeOffset.
const double shadowTopTX = -0.01171875;     // From shapeOffset.
const double shadowTopTY = 0.03125;         // From shapeOffset.
const double shadowBottomTX = -0.01171875;  // From shapeOffset.
const double shadowBottomTY = -0.00390625;  // From shapeOffset.

// Sizes.
const int width = textureSize;
const int height = textureSize;
const int size = width * height;
const double imageScale = 255.0 / width;

// Shape buffers.
static uint shapeData[size];
static double shapeNormalized[size];

// Distance field buffers.
static double distanceIn[size];
static double distanceOut[size];

// Temporary buffers used by the computegradient() and edtaa3() functions exposed by edtaa3func.c.
static short distanceX[size];
static short distanceY[size];
static double gradientX[size];
static double gradientY[size];

// Final image buffer.
static uint imageData[size];

int main(int argc, char* argv[])
{
    if (argc != 3) {
        qWarning("Usage: createshapeimage input_svg output_cpp");
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

    QImage shape(reinterpret_cast<uchar*>(shapeData), width, height, width * 4,
                 QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&shape);

    // Render and store the distance field used for masking the top of the shape, for masking the
    // bevel and for masking the unstyled shape.
    painter.translate(shapeOffset * width, shapeOffset * width);
    svg.render(&painter);
    for (int i = 0; i < size; i++) {
        shapeNormalized[i] = static_cast<double>(shapeData[i] >> 24) / 255.0;
    }
    computegradient(shapeNormalized, width, height, gradientX, gradientY);
    edtaa3(shapeNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceOut);
    for (int i = 0; i < size; i++) {
        shapeNormalized[i] = 1.0 - shapeNormalized[i];
    }
    memset(gradientX, 0, size * sizeof(double));
    memset(gradientY, 0, size * sizeof(double));
    computegradient(shapeNormalized, width, height, gradientX, gradientY);
    edtaa3(shapeNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        const double distance = qMax(0.0, distanceIn[i]) - qMax(0.0, distanceOut[i]);
        const uint value = qBound(0, qRound(distance * distanceScale * imageScale + 127.5), 255);
        imageData[i] = value << 16;  // Stored in channel R (exposed as B in the shaders).
    }

    // Render and store the distance field used for masking the bottom of the shape and for masking
    // the bevel.
    memset(shapeData, 0, size * sizeof(uint));
    painter.resetTransform();
    painter.translate(shapeOffset * width, (shapeOffset + distanceBottomTY) * width);
    svg.render(&painter);
    for (int i = 0; i < size; i++) {
        shapeNormalized[i] = static_cast<double>(shapeData[i] >> 24) / 255.0;
    }
    computegradient(shapeNormalized, width, height, gradientX, gradientY);
    edtaa3(shapeNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceOut);
    for (int i = 0; i < size; i++) {
        shapeNormalized[i] = 1.0 - shapeNormalized[i];
    }
    memset(gradientX, 0, size * sizeof(double));
    memset(gradientY, 0, size * sizeof(double));
    computegradient(shapeNormalized, width, height, gradientX, gradientY);
    edtaa3(shapeNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        const double distance = qMax(0.0, distanceIn[i]) - qMax(0.0, distanceOut[i]);
        const uint value = qBound(0, qRound(distance * distanceScale * imageScale + 127.5), 255);
        imageData[i] |= value << 24;  // Stored in channel A.
    }

    // Render and store the top inner shadow.
    memset(shapeData, 0, size * sizeof(uint));
    painter.resetTransform();
    painter.translate((shapeOffset + shadowTopTX) * width, (shapeOffset + shadowTopTY) * width);
    svg.render(&painter);
    for (int i = 0; i < size; i++) {
        shapeNormalized[i] = 1.0 - (static_cast<double>(shapeData[i] >> 24) / 255.0);
    }
    computegradient(shapeNormalized, width, height, gradientX, gradientY);
    edtaa3(shapeNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        double shadow = qBound(0.0, (distanceIn[i] * shadowScale * imageScale) / 255.0, 1.0);
        shadow = (1.0 - (2.0 * shadow - shadow * shadow)) * 255.0;
        const uint value = qBound(0, qRound(shadow * shadowTranslucency), 255);
        imageData[i] |= value << 0;  // Stored in channel B (exposed as R in the shaders).
    }

    // Render and store the bottom inner shadow.
    memset(shapeData, 0, size * sizeof(uint));
    painter.resetTransform();
    painter.translate(
        (shapeOffset + shadowBottomTX) * width, (shapeOffset + shadowBottomTY) * width);
    svg.render(&painter);
    for (int i = 0; i < size; i++) {
        shapeNormalized[i] = 1.0 - (static_cast<double>(shapeData[i] >> 24) / 255.0);
    }
    computegradient(shapeNormalized, width, height, gradientX, gradientY);
    edtaa3(shapeNormalized, gradientX, gradientY, width, height, distanceX, distanceY,
           distanceIn);
    for (int i = 0; i < size; i++) {
        double shadow = qBound(0.0, (distanceIn[i] * shadowScale * imageScale) / 255.0, 1.0);
        shadow = (1.0 - (2.0 * shadow - shadow * shadow)) * 255.0;
        const uint value = qBound(0, qRound(shadow * shadowTranslucency), 255);
        imageData[i] |= value << 8;  // Stored in channel G.
    }

    // Write the C++ file.
    QTextStream cppOut(&cppFile);
    cppOut << "// Copyright 2015 Canonical Ltd.\n"
           << "// Automatically generated by the createshapeimage tool.\n"
           << "\n"
           << "static const struct {\n"
           << "    float offset;\n"
           << "    float distanceAA;\n"
           << "    int size;\n"
           << "    unsigned char data[" << size * 4 + 1 << "];\n"  // + 1 for the string terminator.
           << "} shapeTextureInfo = {\n"
           << "    " << shapeOffset << ",\n"
           << "    " << distanceScale << ",\n"
           << "    " << width << ",\n";
    cppOut.setIntegerBase(16);
    cppOut.setFieldWidth(2);
    cppOut.setPadChar('0');
    for (int i = 0; i < size; i += 4) {
        cppOut << "    \""
                << "\\x" << (imageData[i] & 0xff)
                << "\\x" << ((imageData[i] >> 8) & 0xff)
                << "\\x" << ((imageData[i] >> 16) & 0xff)
                << "\\x" << ((imageData[i] >> 24) & 0xff)
                << "\\x" << (imageData[i+1] & 0xff)
                << "\\x" << ((imageData[i+1] >> 8) & 0xff)
                << "\\x" << ((imageData[i+1] >> 16) & 0xff)
                << "\\x" << ((imageData[i+1] >> 24) & 0xff)
                << "\\x" << (imageData[i+2] & 0xff)
                << "\\x" << ((imageData[i+2] >> 8) & 0xff)
                << "\\x" << ((imageData[i+2] >> 16) & 0xff)
                << "\\x" << ((imageData[i+2] >> 24) & 0xff)
                << "\\x" << (imageData[i+3] & 0xff)
                << "\\x" << ((imageData[i+3] >> 8) & 0xff)
                << "\\x" << ((imageData[i+3] >> 16) & 0xff)
                << "\\x" << ((imageData[i+3] >> 24) & 0xff);
        cppOut.setFieldWidth(1);
        cppOut << "\"\n";
        cppOut.setFieldWidth(2);
    }
    cppOut << "};\n";

    // Save the file as a PNG for debugging purpose.
    // QImage image(reinterpret_cast<uchar*>(imageData), width, height, width * 4,
    //              QImage::Format_ARGB32);
    // image.save("test.png");

    return 0;
}
