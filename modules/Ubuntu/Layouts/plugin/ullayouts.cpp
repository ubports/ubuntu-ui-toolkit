/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ullayouts.h"
#include "ullayouts_p.h"

ULLayoutsPrivate::ULLayoutsPrivate(ULLayouts *qq) :
    q_ptr(qq)
{
}

ULLayouts::ULLayouts(QQuickItem *parent):
    QQuickItem(parent),
    d_ptr(new ULLayoutsPrivate(this))
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()
    
    // setFlag(ItemHasContents, true);
}

ULLayouts::~ULLayouts()
{
}

QString ULLayouts::currentLayout() const
{
    return QString();
}

QQmlListProperty<ULConditionalLayout> ULLayouts::layouts()
{
    return QQmlListProperty<ULConditionalLayout>();
}
