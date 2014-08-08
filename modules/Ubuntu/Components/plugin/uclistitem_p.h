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
#include <QtCore/QPointer>

class QQuickFlickable;
class UCListItemBackground;
class UCListItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCListItemBase)
public:
    UCListItemBasePrivate(UCListItemBase *qq);

    static inline UCListItemBasePrivate *get(UCListItemBase *that)
    {
        Q_ASSERT(that);
        return that->d_ptr.data();
    }
    void _q_rebound();
    void setPressed(bool pressed);
    void listenToRebind(bool listen);

    UCListItemBase *q_ptr;
    QPointer<QQuickFlickable> flickable;
    UCListItemBackground *background;
    bool pressed:1;
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

QML_DECLARE_TYPE(UCListItemBackground)

#endif // UCVIEWITEM_P_H
