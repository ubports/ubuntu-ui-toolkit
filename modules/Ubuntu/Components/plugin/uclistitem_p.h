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
#include "ucglobals.h"
#include "ucfocusscope_p.h"
#include <QtCore/QPointer>
#include <QtQuick/private/qquickrectangle_p.h>

class QQuickFlickable;
class UCListItemBackground;
class UCListItemDivider;
class UCViewItemOptions;
class UCListItemBasePrivate : public UCFocusScopePrivate
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
    void setPressed(bool pressed);
    void listenToRebind(bool listen);
    void resize();

    QPointer<QQuickFlickable> flickable;
    UCListItemBackground *background;
    UCListItemDivider *divider;
    UCViewItemOptions *leadingOptions;
    UCViewItemOptions *trailingOptions;
};

class UCListItemBackground : public QQuickItem
{
    Q_OBJECT
    DECLARE_PROPERTY(QColor, color)
    DECLARE_PROPERTY(QColor, pressedColor)
public:
    explicit UCListItemBackground(QQuickItem *parent = 0);
    ~UCListItemBackground();

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);

private:
    UCListItemBase *m_item;
};

class UCListItemDivider : public QObject
{
    Q_OBJECT
    DECLARE_PROPERTY(bool, visible)
    DECLARE_PROPERTY(qreal, thickness)
    DECLARE_PROPERTY(qreal, leftMargin)
    DECLARE_PROPERTY(qreal, rightMargin)
    DECLARE_PROPERTY_PTYPE(QQuickGradient, gradient)
    DECLARE_PROPERTY(QColor, color)
public:
    explicit UCListItemDivider(QObject *parent = 0);
    ~UCListItemDivider();
    void init(UCListItemBase *listItem);

protected:
    QSGNode *paint(QSGNode *paintNode, const QRectF &rect);

private:
    void resizeAndUpdate() {
        UCListItemBasePrivate::get(m_listItem)->resize();
        m_listItem->update();
    }

    UCListItemBase *m_listItem;
    friend class UCListItemBase;
    friend class UCListItemBasePrivate;
};

QML_DECLARE_TYPE(UCListItemBackground)
QML_DECLARE_TYPE(UCListItemDivider)

#endif // UCVIEWITEM_P_H
