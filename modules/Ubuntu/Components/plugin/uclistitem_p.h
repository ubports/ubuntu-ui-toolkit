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
 */

#ifndef UCVIEWITEM_P_H
#define UCVIEWITEM_P_H

#include "uclistitem.h"
#include "ucstyleditembase_p.h"
#include <QtCore/QPointer>
#include <QtQuick/private/qquickrectangle_p.h>

class QQuickFlickable;
class QQuickPropertyAnimation;
class UCListItemContent;
class UCListItemDivider;
class UCListItemOptions;
class PropertyChange;
class UCListItemPrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCListItem)
public:
    UCListItemPrivate();
    virtual ~UCListItemPrivate();
    void init();

    static inline UCListItemPrivate *get(UCListItem *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    // override setFocusable()
    void setFocusable();

    void _q_updateColors();
    void _q_rebound();
    void _q_updateSize();
    void _q_completeRebinding();
    void cleanup();
    void reboundTo(qreal x);
    void setPressed(bool pressed);
    void setMoved(bool moved);
    bool grabPanel(UCListItemOptions *optionList, bool isMoved);
    void listenToRebind(bool listen);
    void resize();
    void update();
    void clampX(qreal &x, qreal dx);

    bool pressed:1;
    bool pressedColorChanged:1;
    bool moved:1;
    bool suppressClick:1;
    bool ready:1;
    qreal xAxisMoveThresholdGU;
    QPointF lastPos;
    QPointF pressedPos;
    QColor color;
    QColor pressedColor;
    QPointer<QQuickFlickable> flickable;
    QQuickPropertyAnimation *reboundAnimation;
    PropertyChange *flickableInteractive;
    QQuickItem *contentItem;
    UCListItemDivider *divider;
    UCListItemOptions *leadingOptions;
    UCListItemOptions *trailingOptions;
};

class UCListItemDivider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible MEMBER m_visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal leftMargin MEMBER m_leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin MEMBER m_rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(QColor colorFrom MEMBER m_colorFrom WRITE setColorFrom NOTIFY colorFromChanged)
    Q_PROPERTY(QColor colorTo MEMBER m_colorTo WRITE setColorTo NOTIFY colorToChanged)
public:
    explicit UCListItemDivider(QObject *parent = 0);
    ~UCListItemDivider();
    void init(UCListItem *listItem);

Q_SIGNALS:
    void visibleChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void colorFromChanged();
    void colorToChanged();

protected:
    QSGNode *paint(const QRectF &rect);

private Q_SLOTS:
    void unitsChanged();
    void paletteChanged();

private:
    void updateGradient();
    void setVisible(bool visible);
    void setLeftMargin(qreal leftMargin);
    void setRightMargin(qreal rightMargin);
    void setColorFrom(const QColor &color);
    void setColorTo(const QColor &color);

    bool m_visible:1;
    bool m_leftMarginChanged:1;
    bool m_rightMarginChanged:1;
    bool m_colorFromChanged:1;
    bool m_colorToChanged:1;
    qreal m_thickness;
    qreal m_leftMargin;
    qreal m_rightMargin;
    QColor m_colorFrom;
    QColor m_colorTo;
    QGradientStops m_gradient;
    UCListItemPrivate *m_listItem;
    friend class UCListItem;
    friend class UCListItemPrivate;
};

QColor getPaletteColor(const char *profile, const char *color);

QML_DECLARE_TYPE(UCListItemDivider)

#endif // UCVIEWITEM_P_H
