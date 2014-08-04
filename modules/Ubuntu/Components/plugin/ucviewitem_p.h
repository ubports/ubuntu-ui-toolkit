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

#include "ucviewitem.h"
#include "ucglobals.h"
#include <QtCore/QPointer>
#include <QtQuick/private/qquickrectangle_p.h>

class QQuickFlickable;
class UCViewItemDivider;
class UCViewItemBackground;
class UCViewItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCViewItemBase)
public:
    UCViewItemBasePrivate(UCViewItemBase *qq);

    static inline UCViewItemBasePrivate *get(UCViewItemBase *that)
    {
        Q_ASSERT(that);
        return that->d_ptr.data();
    }
    void _q_rebound();
    void setPressed(bool pressed);
    void listenToRebind(bool listen);
    void resize();

    UCViewItemBase *q_ptr;
    QPointer<QQuickFlickable> flickable;
    UCViewItemBackground *background;
    UCViewItemDivider *divider;
    bool pressed:1;
};

class UCViewItemBackground : public QQuickItem
{
    Q_OBJECT
    DECLARE_PROPERTY(QColor, color, update())
    DECLARE_PROPERTY(QColor, pressedColor, update())
public:
    explicit UCViewItemBackground(QQuickItem *parent = 0);
    ~UCViewItemBackground();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);

private:
    UCViewItemBase *m_item;
};

class UCViewItemDivider : public QObject
{
    Q_OBJECT
    DECLARE_PROPERTY(bool, visible, resizeAndUpdate())
    DECLARE_PROPERTY(qreal, thickness, resizeAndUpdate())
    DECLARE_PROPERTY(qreal, leftMargin, update())
    DECLARE_PROPERTY(qreal, rightMargin, update())
    DECLARE_PROPERTY_PTR(QQuickGradient, gradient, gradientUpdate())
public:
    explicit UCViewItemDivider(UCViewItemBase *viewItem);
    ~UCViewItemDivider();

protected:
    QSGNode *paint(QSGNode *paintNode, const QRectF &rect);

private:
    void update() {
        m_viewItem->update();
    }

    void resizeAndUpdate() {
        UCViewItemBasePrivate::get(m_viewItem)->resize();
        m_viewItem->update();
    }

    void gradientUpdate() {

    }

    UCViewItemBase *m_viewItem;
    friend class UCViewItemBase;
    friend class UCViewItemBasePrivate;
};

QML_DECLARE_TYPE(UCViewItemDivider)
QML_DECLARE_TYPE(UCViewItemBackground)

#endif // UCVIEWITEM_P_H
