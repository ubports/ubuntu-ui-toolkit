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

#ifndef FRAME_H
#define FRAME_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGNode>

class UCFrameMaterial : public QSGMaterial
{
public:
    UCFrameMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;

    quint32 textureId() const { return m_textureId; }

private:
    quint32 m_textureId;
};

class UCFrameNode : public QSGGeometryNode
{
public:
  struct Vertex { float x, y, s1, t1, s2, t2; quint32 color; };

    static const unsigned short* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCFrameNode();
    void updateGeometry(const QSizeF& itemSize, float thickness, float radius, QRgb color);

private:
    UCFrameMaterial m_material;
    QSGGeometry m_geometry;
};

// Renders the frame (border) of a shape.
class UCFrame : public QQuickItem
{
    Q_OBJECT

    // Thickness of the frame in pixels.
    Q_PROPERTY(qreal thickness READ thickness WRITE setThickness NOTIFY thicknessChanged)

    // Radius of the shape in pixels. A rectangle frame could be obtained using
    // a radius of 0 but it's recommended to use a Ractangle with a border for
    // that as it is a bit more efficient in term of rendering speed (reason is
    // we don't bother to specify a dedicated mesh and shader when radius is 0).
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

    // Color of the frame. Translucent colors are supported too.
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    UCFrame(QQuickItem* parent = 0);

    qreal thickness() const { return m_thickness; }
    void setThickness(qreal thickness);
    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);
    QColor color() const {
      return QColor(qRed(m_color), qGreen(m_color), qBlue(m_color), qAlpha(m_color)); }
    void setColor(const QColor& color);

Q_SIGNALS:
    void thicknessChanged();
    void radiusChanged();
    void colorChanged();

private:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

    QRgb m_color;
    float m_thickness;
    float m_radius;

    Q_DISABLE_COPY(UCFrame)
};

QML_DECLARE_TYPE(UCFrame)

#endif  // FRAME_H
