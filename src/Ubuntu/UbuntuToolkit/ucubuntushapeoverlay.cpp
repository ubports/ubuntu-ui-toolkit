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

// FIXME(loicm) Overlay data is passed as geometry and interpolated as varying so that it can be
//     batched. It would be very useful to implement it with uniforms and compare the perf by
//     plotting the rendering speed of a scene with an increasing number of overlaid shapes.

#include "ucubuntushapeoverlay_p.h"

// -- Scene graph shader ---

UT_NAMESPACE_BEGIN

ShapeOverlayShader::ShapeOverlayShader()
{
    setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/uc/shaders/shapeoverlay.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment, useDistanceFields() ?
                        QStringLiteral(":/uc/shaders/shapeoverlay.frag") :
                        QStringLiteral(":/uc/shaders/shapeoverlay_mipmap.frag"));
}

char const* const* ShapeOverlayShader::attributeNames() const
{
    static char const* const attributes[] = {
        "positionAttrib", "shapeCoordAttrib", "sourceCoordAttrib", "yCoordAttrib",
        "backgroundColorAttrib", "overlayCoordAttrib", "overlayColorAttrib", 0
    };
    return attributes;
}

// --- Scene graph material ---

QSGMaterialType* ShapeOverlayMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader* ShapeOverlayMaterial::createShader() const
{
    return new ShapeOverlayShader;
}

// --- Scene graph node ---

ShapeOverlayNode::ShapeOverlayNode()
    : QSGGeometryNode()
    , m_material()
    , m_geometry(attributeSet(), ShapeNode::vertexCount, ShapeNode::indexCount,
                 ShapeNode::indexType)
{
    QSGNode::setFlag(UsePreprocess, true);
    memcpy(m_geometry.indexData(), ShapeNode::indices(),
           ShapeNode::indexCount * ShapeNode::indexTypeSize);
    m_geometry.setDrawingMode(ShapeNode::drawingMode);
    m_geometry.setIndexDataPattern(ShapeNode::indexDataPattern);
    m_geometry.setVertexDataPattern(ShapeNode::vertexDataPattern);
    setMaterial(&m_material);
    setGeometry(&m_geometry);
#ifdef QSG_RUNTIME_DESCRIPTION
    qsgnode_set_description(this, QLatin1String("ubuntushapeoverlay"));
#endif
}

// static
const QSGGeometry::AttributeSet& ShapeOverlayNode::attributeSet()
{
    static const QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 2, GL_FLOAT, true),
        QSGGeometry::Attribute::create(1, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(2, 4, GL_FLOAT),
        QSGGeometry::Attribute::create(3, 1, GL_FLOAT),
        QSGGeometry::Attribute::create(4, 4, GL_UNSIGNED_BYTE),
        QSGGeometry::Attribute::create(5, 2, GL_FLOAT),
        QSGGeometry::Attribute::create(6, 4, GL_UNSIGNED_BYTE)
    };
    static const QSGGeometry::AttributeSet attributeSet = {
        7, sizeof(Vertex), attributes
    };
    return attributeSet;
}

// --- QtQuick item ---

/*! \qmltype UbuntuShapeOverlay
    \instantiates UCUbuntuShapeOverlay
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \brief Extended UbuntuShape adding a colored overlay layer.

    The UbuntuShapeOverlay is a rounded rectangle (based on a \l
    {https://en.wikipedia.org/wiki/Squircle}{squircle}) containing a set of layers composed, from
    top to bottom, of a colored rectangle overlay, an optional source image and a background color
    (solid or linear gradient). Different properties allow to change the look of the shape.

    Example:

    \qml
    import Ubuntu.Components 1.2

    UbuntuShapeOverlay {
        backgroundColor: "white"
        overlayColor: "black"
        overlayRect: Qt.rect(0.25, 0.25, 0.5, 0.5)
    }
    \endqml
*/
UCUbuntuShapeOverlay::UCUbuntuShapeOverlay(QQuickItem* parent)
    : UCUbuntuShape(parent)
    , m_overlayX(0)
    , m_overlayY(0)
    , m_overlayWidth(0)
    , m_overlayHeight(0)
    , m_overlayColor(qRgba(0, 0, 0, 0))
{
}

/*! \qmlproperty rect UbuntuShapeOverlay::overlayRect
    \since Ubuntu.Components 1.2

    This property sets the overlay rectangle. The default value is the empty rectangle.

    It is defined by a position and a size in normalized item coordinates (in the range between 0
    and 1) with the origin at the top-left corner. An overlay covering the bottom part and starting
    at the vertical center can be done like that:

    \qml
    UbuntuShapeOverlay {
        width: 200; height: 200
        overlayColor: Qt.rgba(0.0, 0.0, 0.5, 0.5)
        overlayRect: Qt.rect(0.0, 0.5, 1.0, 0.5)
    }
    \endqml

    Converting a position and a size in pixels to normalized item coordinates can be done with a
    division by the size. Here is an equivalent of the previous code sample:

    \qml
    UbuntuShapeOverlay {
        width: 200; height: 200
        overlayColor: Qt.rgba(0.0, 0.0, 0.5, 0.5)
        overlayRect: Qt.rect(100.0/width, 100.0/height, 200.0/width, 100.0/height)
    }
    \endqml

    A geometry exceeding the item area is cropped.
*/
void UCUbuntuShapeOverlay::setOverlayRect(const QRectF& overlayRect)
{
    // Crop rectangle and pack to 16-bit unsigned integers.
    const float x = qBound(0.0f, static_cast<float>(overlayRect.x()), 1.0f);
    float width = qMax(0.0f, static_cast<float>(overlayRect.width()));
    if ((x + width) > 1.0f) {
        width += 1.0f - (x + width);
    }
    const float y = qBound(0.0f, static_cast<float>(overlayRect.y()), 1.0f);
    float height = qMax(0.0f, static_cast<float>(overlayRect.height()));
    if ((y + height) > 1.0f) {
        height += 1.0f - (y + height);
    }
    const float f32toU16 = static_cast<float>(0xffff);
    const quint16 overlayX = static_cast<quint16>(x * f32toU16);
    const quint16 overlayY = static_cast<quint16>(y * f32toU16);
    const quint16 overlayWidth = static_cast<quint16>(width * f32toU16);
    const quint16 overlayHeight = static_cast<quint16>(height * f32toU16);

    if ((m_overlayX != overlayX) || (m_overlayY != overlayY) ||
        (m_overlayWidth != overlayWidth) || (m_overlayHeight != overlayHeight)) {
        m_overlayX = overlayX;
        m_overlayY = overlayY;
        m_overlayWidth = overlayWidth;
        m_overlayHeight = overlayHeight;
        update();
        Q_EMIT overlayRectChanged();
    }
}

/*! \qmlproperty color UbuntuShapeOverlay::overlayColor
    \since Ubuntu.Components 1.2

    This property sets the color of the overlay rectangle defined by \l overlayRect. The default
    value is transparent black.
*/
void UCUbuntuShapeOverlay::setOverlayColor(const QColor& overlayColor)
{
    const QRgb overlayColorRgb = qRgba(
        overlayColor.red(), overlayColor.green(), overlayColor.blue(), overlayColor.alpha());
    if (m_overlayColor != overlayColorRgb) {
        m_overlayColor = overlayColorRgb;
        update();
        Q_EMIT overlayColorChanged();
    }
}

QSGNode* UCUbuntuShapeOverlay::createSceneGraphNode() const
{
    return new ShapeOverlayNode;
}

// Pack to a premultiplied 32-bit ABGR integer.
static quint32 packColor(QRgb color)
{
    const quint32 a = qAlpha(color);
    const quint32 b = ((qBlue(color) * a) + 0xff) >> 8;
    const quint32 g = ((qGreen(color) * a) + 0xff) >> 8;
    const quint32 r = ((qRed(color) * a) + 0xff) >> 8;
    return (a << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);
}

void UCUbuntuShapeOverlay::updateGeometry(
    QSGNode* node, const QSizeF& itemSize, float radius, float shapeOffset,
    const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
    const quint32 backgroundColor[3])
{
    ShapeOverlayNode::Vertex* v = reinterpret_cast<ShapeOverlayNode::Vertex*>(
        static_cast<ShapeOverlayNode*>(node)->geometry()->vertexData());

    // Get the affine transformation for the overlay coordinates, pixels lying inside the mask
    // (values in the range [-1, 1]) will be considered overlaid in the fragment shader.
    const float u16toF32 = 1.0f / static_cast<float>(0xffff);
    const float invOverlayWidth = 1.0f / (m_overlayWidth * u16toF32);
    const float invOverlayHeight = 1.0f / (m_overlayHeight * u16toF32);
    const float overlaySx = invOverlayWidth * 2.0f;
    const float overlaySy = invOverlayHeight * 2.0f;
    const float overlayTx = -((m_overlayX * u16toF32) * invOverlayWidth) * 2.0f - 1.0f;
    const float overlayTy = -((m_overlayY * u16toF32) * invOverlayHeight) * 2.0f - 1.0f;

    // The overlay affine transformation doesn't exist when width or height is null, whenever that
    // is the case we simply set the color to transparent. GPUs handle NaN/Inf values flowlessly.
    const quint32 overlayColor = qIsFinite(invOverlayHeight + invOverlayWidth) ?
        packColor(m_overlayColor) : 0x00000000;

    // Set top row of 3 vertices.
    v[0].position[0] = 0.0f;
    v[0].position[1] = 0.0f;
    v[0].shapeCoordinate[0] = shapeOffset;
    v[0].shapeCoordinate[1] = shapeOffset;
    v[0].sourceCoordinate[0] = sourceCoordTransform.z();
    v[0].sourceCoordinate[1] = sourceCoordTransform.w();
    v[0].sourceCoordinate[2] = sourceMaskTransform.z();
    v[0].sourceCoordinate[3] = sourceMaskTransform.w();
    v[0].yCoordinate = -1.0f;
    v[0].backgroundColor = backgroundColor[0];
    v[0].overlayCoordinate[0] = overlayTx;
    v[0].overlayCoordinate[1] = overlayTy;
    v[0].overlayColor = overlayColor;
    v[1].position[0] = 0.5f * itemSize.width();
    v[1].position[1] = 0.0f;
    v[1].shapeCoordinate[0] = (0.5f * itemSize.width()) / radius - shapeOffset;
    v[1].shapeCoordinate[1] = shapeOffset;
    v[1].sourceCoordinate[0] = 0.5f * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[1].sourceCoordinate[1] = sourceCoordTransform.w();
    v[1].sourceCoordinate[2] = 0.5f * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[1].sourceCoordinate[3] = sourceMaskTransform.w();
    v[1].yCoordinate = -1.0f;
    v[1].backgroundColor = backgroundColor[0];
    v[1].overlayCoordinate[0] = 0.5f * overlaySx + overlayTx;
    v[1].overlayCoordinate[1] = overlayTy;
    v[1].overlayColor = overlayColor;
    v[2].position[0] = itemSize.width();
    v[2].position[1] = 0.0f;
    v[2].shapeCoordinate[0] = shapeOffset;
    v[2].shapeCoordinate[1] = shapeOffset;
    v[2].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[2].sourceCoordinate[1] = sourceCoordTransform.w();
    v[2].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[2].sourceCoordinate[3] = sourceMaskTransform.w();
    v[2].yCoordinate = -1.0f;
    v[2].backgroundColor = backgroundColor[0];
    v[2].overlayCoordinate[0] = overlaySx + overlayTx;
    v[2].overlayCoordinate[1] = overlayTy;
    v[2].overlayColor = overlayColor;

    // Set middle row of 3 vertices.
    v[3].position[0] = 0.0f;
    v[3].position[1] = 0.5f * itemSize.height();
    v[3].shapeCoordinate[0] = shapeOffset;
    v[3].shapeCoordinate[1] = (0.5f * itemSize.height()) / radius - shapeOffset;
    v[3].sourceCoordinate[0] = sourceCoordTransform.z();
    v[3].sourceCoordinate[1] = 0.5f * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[3].sourceCoordinate[2] = sourceMaskTransform.z();
    v[3].sourceCoordinate[3] = 0.5f * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[3].yCoordinate = 0.0f;
    v[3].backgroundColor = backgroundColor[1];
    v[3].overlayCoordinate[0] = overlayTx;
    v[3].overlayCoordinate[1] = 0.5f * overlaySy + overlayTy;
    v[3].overlayColor = overlayColor;
    v[4].position[0] = 0.5f * itemSize.width();
    v[4].position[1] = 0.5f * itemSize.height();
    v[4].shapeCoordinate[0] = (0.5f * itemSize.width()) / radius - shapeOffset;
    v[4].shapeCoordinate[1] = (0.5f * itemSize.height()) / radius - shapeOffset;
    v[4].sourceCoordinate[0] = 0.5f * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[4].sourceCoordinate[1] = 0.5f * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[4].sourceCoordinate[2] = 0.5f * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[4].sourceCoordinate[3] = 0.5f * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[4].yCoordinate = 0.0f;
    v[4].backgroundColor = backgroundColor[1];
    v[4].overlayCoordinate[0] = 0.5f * overlaySx + overlayTx;
    v[4].overlayCoordinate[1] = 0.5f * overlaySy + overlayTy;
    v[4].overlayColor = overlayColor;
    v[5].position[0] = itemSize.width();
    v[5].position[1] = 0.5f * itemSize.height();
    v[5].shapeCoordinate[0] = shapeOffset;
    v[5].shapeCoordinate[1] = (0.5f * itemSize.height()) / radius - shapeOffset;
    v[5].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[5].sourceCoordinate[1] = 0.5f * sourceCoordTransform.y() + sourceCoordTransform.w();
    v[5].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[5].sourceCoordinate[3] = 0.5f * sourceMaskTransform.y() + sourceMaskTransform.w();
    v[5].yCoordinate = 0.0f;
    v[5].backgroundColor = backgroundColor[1];
    v[5].overlayCoordinate[0] = overlaySx + overlayTx;
    v[5].overlayCoordinate[1] = 0.5f * overlaySy + overlayTy;
    v[5].overlayColor = overlayColor;

    // Set bottom row of 3 vertices.
    v[6].position[0] = 0.0f;
    v[6].position[1] = itemSize.height();
    v[6].shapeCoordinate[0] = shapeOffset;
    v[6].shapeCoordinate[1] = shapeOffset;
    v[6].sourceCoordinate[0] = sourceCoordTransform.z();
    v[6].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[6].sourceCoordinate[2] = sourceMaskTransform.z();
    v[6].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[6].yCoordinate = 1.0f;
    v[6].backgroundColor = backgroundColor[2];
    v[6].overlayCoordinate[0] = overlayTx;
    v[6].overlayCoordinate[1] = overlaySy + overlayTy;
    v[6].overlayColor = overlayColor;
    v[7].position[0] = 0.5f * itemSize.width();
    v[7].position[1] = itemSize.height();
    v[7].shapeCoordinate[0] = (0.5f * itemSize.width()) / radius - shapeOffset;
    v[7].shapeCoordinate[1] = shapeOffset;
    v[7].sourceCoordinate[0] = 0.5f * sourceCoordTransform.x() + sourceCoordTransform.z();
    v[7].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[7].sourceCoordinate[2] = 0.5f * sourceMaskTransform.x() + sourceMaskTransform.z();
    v[7].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[7].yCoordinate = 1.0f;
    v[7].backgroundColor = backgroundColor[2];
    v[7].overlayCoordinate[0] = 0.5f * overlaySx + overlayTx;
    v[7].overlayCoordinate[1] = overlaySy + overlayTy;
    v[7].overlayColor = overlayColor;
    v[8].position[0] = itemSize.width();
    v[8].position[1] = itemSize.height();
    v[8].shapeCoordinate[0] = shapeOffset;
    v[8].shapeCoordinate[1] = shapeOffset;
    v[8].sourceCoordinate[0] = sourceCoordTransform.x() + sourceCoordTransform.z();
    v[8].sourceCoordinate[1] = sourceCoordTransform.y() + sourceCoordTransform.w();
    v[8].sourceCoordinate[2] = sourceMaskTransform.x() + sourceMaskTransform.z();
    v[8].sourceCoordinate[3] = sourceMaskTransform.y() + sourceMaskTransform.w();
    v[8].yCoordinate = 1.0f;
    v[8].backgroundColor = backgroundColor[2];
    v[8].overlayCoordinate[0] = overlaySx + overlayTx;
    v[8].overlayCoordinate[1] = overlaySy + overlayTy;
    v[8].overlayColor = overlayColor;

    node->markDirty(QSGNode::DirtyGeometry);
}

UT_NAMESPACE_END
