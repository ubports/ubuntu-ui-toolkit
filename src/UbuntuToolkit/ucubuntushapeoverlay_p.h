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

#ifndef UCUBUNTUSHAPEOVERLAY_H
#define UCUBUNTUSHAPEOVERLAY_H

#include "ucubuntushape_p.h"

// --- Scene graph shader ---

UT_NAMESPACE_BEGIN

class ShapeOverlayShader : public ShapeShader
{
public:
    ShapeOverlayShader();
    char const* const* attributeNames() const override;
};

// --- Scene graph material ---

class ShapeOverlayMaterial : public ShapeMaterial
{
public:
    QSGMaterialType* type() const override;
    QSGMaterialShader* createShader() const override;
};

// --- Scene graph node ---

class ShapeOverlayNode : public QSGGeometryNode
{
public:
    struct Vertex {
        float position[2];
        float shapeCoordinate[2];
        float sourceCoordinate[4];
        float yCoordinate;
        quint32 backgroundColor;
        float overlayCoordinate[2];
        quint32 overlayColor;
    };

    static const QSGGeometry::AttributeSet& attributeSet();

    ShapeOverlayNode();
    QSGGeometry* geometry() { return &m_geometry; }
    void preprocess() override { m_material.updateTextures(); }

private:
    ShapeOverlayMaterial m_material;
    QSGGeometry m_geometry;
};

// --- QtQuick item ---

class UBUNTUTOOLKIT_EXPORT UCUbuntuShapeOverlay : public UCUbuntuShape
{
    Q_OBJECT

    // Overlay properties.
    Q_PROPERTY(QRectF overlayRect READ overlayRect WRITE setOverlayRect NOTIFY overlayRectChanged)
    Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor
               NOTIFY overlayColorChanged)

public:
    UCUbuntuShapeOverlay(QQuickItem* parent=0);

    QRectF overlayRect() const {
        const float u16ToF32 = 1.0f / static_cast<float>(0xffff);
        return QRectF(m_overlayX * u16ToF32, m_overlayY * u16ToF32, m_overlayWidth * u16ToF32,
                      m_overlayHeight * u16ToF32); }
    void setOverlayRect(const QRectF& overlayRect);
    QColor overlayColor() const { return m_overlayColor; }
    void setOverlayColor(const QColor& overlayColor);

Q_SIGNALS:
    void overlayRectChanged();
    void overlayColorChanged();

protected:
    QSGNode* createSceneGraphNode() const override;
    void updateGeometry(
        QSGNode* node, const QSizeF& itemSize, float radius, float shapeOffset,
        const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
        const quint32 backgroundColor[3]) override;

private:
    quint16 m_overlayX;
    quint16 m_overlayY;
    quint16 m_overlayWidth;
    quint16 m_overlayHeight;
    QRgb m_overlayColor;

    Q_DISABLE_COPY(UCUbuntuShapeOverlay)
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(UCUbuntuShapeOverlay))

#endif  // UCUBUNTUSHAPEOVERLAY_H
