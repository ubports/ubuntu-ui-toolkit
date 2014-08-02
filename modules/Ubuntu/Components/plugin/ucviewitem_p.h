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

class QQuickFlickable;
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

    UCViewItemBase *q_ptr;
    QPointer<QQuickFlickable> flickable;
    UCViewItemBackground *background;
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
QML_DECLARE_TYPE(UCViewItemBackground)

#endif // UCVIEWITEM_P_H
