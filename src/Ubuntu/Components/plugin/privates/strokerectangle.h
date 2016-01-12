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

#ifndef UCSTROKERECTANGLE_H
#define UCSTROKERECTANGLE_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGNode>

class UCStrokeRectangleMaterial : public QSGMaterial
{
public:
    UCStrokeRectangleMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;
};

class UCStrokeRectangleNode : public QSGGeometryNode
{
public:
    struct Vertex { float x, y; quint32 color; };

    static const unsigned short* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCStrokeRectangleNode();
    void updateGeometry(const QSizeF& itemSize, float strokeSize, QRgb color);

private:
    UCStrokeRectangleMaterial m_material;
    QSGGeometry m_geometry;
};

class UCStrokeRectangle : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    UCStrokeRectangle(QQuickItem* parent = 0);

    qreal size() const { return m_size; }
    void setSize(qreal size);
    QColor color() const {
      return QColor(qRed(m_color), qGreen(m_color), qBlue(m_color), qAlpha(m_color)); }
    void setColor(const QColor& color);

Q_SIGNALS:
    void sizeChanged();
    void colorChanged();

private:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

    QRgb m_color;
    float m_size;

    Q_DISABLE_COPY(UCStrokeRectangle)
};

QML_DECLARE_TYPE(UCStrokeRectangle)

#endif  // UCSTROKERECTANGLE_H
