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
    q_ptr(qq),
    currentLayout(0),
    currentLayoutIndex(-1)
{
}

void ULLayoutsPrivate::append_layout(QQmlListProperty<ULConditionalLayout> *list, ULConditionalLayout *layout)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    if (layout) {
        layout->setParent(_this);
//        layout->setLayoutManager(_this);
        _this->d_ptr->layouts.append(layout);
    }
}

int ULLayoutsPrivate::count_layouts(QQmlListProperty<ULConditionalLayout> *list)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    return _this->d_ptr->layouts.count();
}

ULConditionalLayout *ULLayoutsPrivate::at_layout(QQmlListProperty<ULConditionalLayout> *list, int index)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    return _this->d_ptr->layouts.at(index);
}

void ULLayoutsPrivate::clear_layouts(QQmlListProperty<ULConditionalLayout> *list)
{
    ULLayouts *_this = static_cast<ULLayouts*>(list->object);
    //FIXME: reset LayoutManager property when removing
    _this->d_ptr->layouts.clear();
}

void ULLayoutsPrivate::reLayout()
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
    Q_D(const ULLayouts);
    return d->currentLayoutIndex >= 0 ? d->layouts[d->currentLayoutIndex]->name() : QString();
}

QList<ULConditionalLayout*> ULLayouts::layoutList()
{
    Q_D(ULLayouts);
    return d->layouts;
}

QQmlListProperty<ULConditionalLayout> ULLayouts::layouts()
{
    Q_D(ULLayouts);
    return QQmlListProperty<ULConditionalLayout>(this, &(d->layouts),
                                                 &ULLayoutsPrivate::append_layout,
                                                 &ULLayoutsPrivate::count_layouts,
                                                 &ULLayoutsPrivate::at_layout,
                                                 &ULLayoutsPrivate::clear_layouts);
}
