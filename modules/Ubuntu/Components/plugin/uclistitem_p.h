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

class QQuickFlickable;
class UCListItemContent;
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
    void setPressed(bool pressed);
    void listenToRebind(bool listen);

    bool pressed:1;
    bool pressedColorChanged:1;
    QColor color;
    QColor pressedColor;
    QPointer<QQuickFlickable> flickable;
    QQuickItem *contentItem;
};

QColor getPaletteColor(const char *profile, const char *color);

#endif // UCVIEWITEM_P_H
