/*
 * Copyright 2014 Canonical Ltd.
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

#include "ucubuntushape.h"

// --- Scene graph shader ---

class ShapeOverlayShader : public ShapeShader
{
public:
    ShapeOverlayShader();
    virtual char const* const* attributeNames() const;
};

// --- Scene graph material ---

class ShapeOverlayMaterial : public ShapeMaterial
{
public:
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
};

// --- Scene graph node ---

class ShapeOverlayNode : public QSGGeometryNode
{
public:
    struct Vertex {
        float position[2];
        float shapeCoordinate[2];
        float sourceCoordinate[4];
        quint32 backgroundColor;
        float overlayCoordinate[2];
        quint32 overlayColor;
    };

    static const QSGGeometry::AttributeSet& attributeSet();

    ShapeOverlayNode();
    QSGGeometry* geometry() { return &geometry_; }

private:
    QSGGeometry geometry_;
    ShapeOverlayMaterial material_;
};

// --- QtQuick item ---

class UCUbuntuShapeOverlay : public UCUbuntuShape
{
    Q_OBJECT

    // Overlay properties.
    Q_PROPERTY(QRectF overlayGeometry READ overlayGeometry WRITE setOverlayGeometry
               NOTIFY overlayGeometryChanged)
    Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor
               NOTIFY overlayColorChanged)

public:
    UCUbuntuShapeOverlay(QQuickItem* parent=0);

    QRectF overlayGeometry() const {
        const float u16ToF32 = 1.0f / static_cast<float>(0xffff);
        return QRectF(overlayX_ * u16ToF32, overlayY_ * u16ToF32, overlayWidth_ * u16ToF32,
                      overlayHeight_ * u16ToF32); }
    void setOverlayGeometry(const QRectF& overlayGeometry);
    QColor overlayColor() const { return overlayColor_; }
    void setOverlayColor(const QColor& overlayColor);

Q_SIGNALS:
    void overlayGeometryChanged();
    void overlayColorChanged();

protected:
    virtual QSGNode* createSceneGraphNode() const;
    virtual void updateGeometry(
        QSGNode* node, float width, float height, float radius, const float shapeCoordinate[][2],
        const QVector4D& sourceCoordTransform, const QVector4D& sourceMaskTransform,
        const quint32 backgroundColor[4]);

private:
    quint16 overlayX_;
    quint16 overlayY_;
    quint16 overlayWidth_;
    quint16 overlayHeight_;
    QRgb overlayColor_;

    Q_DISABLE_COPY(UCUbuntuShapeOverlay)
};

QML_DECLARE_TYPE(UCUbuntuShapeOverlay)

#endif  // UCUBUNTUSHAPEOVERLAY_H
