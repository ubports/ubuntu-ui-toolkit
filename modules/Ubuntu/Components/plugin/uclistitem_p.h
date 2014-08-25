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
class UCListItemBackground;
class UCListItemDivider;
class UCListItemOptions;
class UCListItemBasePrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCListItemBase)
public:
    UCListItemBasePrivate();
    virtual ~UCListItemBasePrivate();
    void init();

    static inline UCListItemBasePrivate *get(UCListItemBase *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    // override setFocusable()
    void setFocusable();

    void _q_rebound();
    void reboundTo(qreal x);
    void setPressed(bool pressed);
    void setMoved(UCListItemOptions *optionList, bool isMoved);
    void listenToRebind(bool listen);
    void resize();

    bool pressed:1;
    bool moved:1;
    bool ready:1;
    qreal xAxisMoveThresholdGU;
    QPointF lastPos;
    QPointF pressedPos;
    QPointer<QQuickFlickable> flickable;
    QQuickPropertyAnimation *reboundAnimation;
    UCListItemBackground *background;
    UCListItemDivider *divider;
    UCListItemOptions *leadingOptions;
    UCListItemOptions *trailingOptions;
};

class UCListItemBackground : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color MEMBER m_color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor pressedColor MEMBER m_pressedColor WRITE setPressedColor NOTIFY pressedColorChanged)
public:
    explicit UCListItemBackground(QQuickItem *parent = 0);
    ~UCListItemBackground();

    void setColor(const QColor &color);
    void setPressedColor(const QColor &color);

Q_SIGNALS:
    void colorChanged();
    void pressedColorChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);

private:
    QColor m_color;
    QColor m_pressedColor;
    UCListItemBase *m_item;
};

class UCListItemDivider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible MEMBER m_visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal leftMargin MEMBER m_leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin MEMBER m_rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
public:
    explicit UCListItemDivider(QObject *parent = 0);
    ~UCListItemDivider();
    void init(UCListItemBase *listItem);

Q_SIGNALS:
    void visibleChanged();
    void leftMarginChanged();
    void rightMarginChanged();

protected:
    QSGNode *paint(QSGNode *paintNode, const QRectF &rect);

private Q_SLOTS:
    void unitsChanged();
    void paletteChanged();

private:
    void resizeAndUpdate() {
        UCListItemBasePrivate::get(m_listItem)->resize();
        m_listItem->update();
    }

    void setVisible(bool visible);
    void setLeftMargin(qreal leftMargin);
    void setRightMargin(qreal rightMargin);

    bool m_visible;
    qreal m_thickness;
    qreal m_leftMargin;
    qreal m_rightMargin;
    QGradientStops m_gradient;
    UCListItemBase *m_listItem;
    friend class UCListItemBase;
    friend class UCListItemBasePrivate;
};

QML_DECLARE_TYPE(UCListItemBackground)
QML_DECLARE_TYPE(UCListItemDivider)

#endif // UCVIEWITEM_P_H
