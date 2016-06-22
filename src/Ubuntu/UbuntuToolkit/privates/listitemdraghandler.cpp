/*
 * Copyright 2015 Canonical Ltd.
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

#include "listitemdraghandler.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include <propertychange.h>
#include <QtQuick/private/qquickanimation_p.h>

UT_NAMESPACE_BEGIN

ListItemDragHandler::ListItemDragHandler(UCListItem *baseItem, UCListItem *listItem)
    : QObject(listItem)
    , listItem(listItem)
    , baseItem(baseItem)
    , baseVisible(nullptr)
{
    targetPos = baseItem->position();
    baseVisible = new PropertyChange(baseItem, "visible");
}

ListItemDragHandler::~ListItemDragHandler()
{
    // make sure the property change object is deleted
    delete baseVisible;
}

void ListItemDragHandler::init()
{
    PropertyChange::setValue(baseVisible, false);
    // position the item and show it
    listItem->setPosition(baseItem->position());
    listItem->setZ(2);
    listItem->setVisible(true);
    // emit draggingChanged() signal
    Q_EMIT listItem->draggingChanged();
}

// handles drop gesture animated if the style has animation defined for it
void ListItemDragHandler::drop()
{
    QQuickPropertyAnimation *animation = UCListItemPrivate::get(listItem)->listItemStyle()->m_dropAnimation;
    if (animation) {
        // complete any previous animation
        animation->complete();

        connect(animation, &QQuickAbstractAnimation::stopped,
                this, &ListItemDragHandler::dropItem, Qt::DirectConnection);
        // force properties to contain only the 'y' coordinate
        animation->setProperties("y");
        animation->setTargetObject(listItem);
        animation->setFrom(listItem->y());
        animation->setTo(targetPos.y());
        animation->start();
    } else {
        dropItem();
    }
}

// private slot connected to the reposition animation to drop item
void ListItemDragHandler::dropItem()
{
    listItem->setVisible(false);
    listItem->deleteLater();
    delete baseVisible;
    baseVisible = 0;
}

// update dragged item with the new target item the dragging is hovered over
void ListItemDragHandler::update(UCListItem *hoveredItem)
{
    if (hoveredItem) {
        targetPos = hoveredItem->position();
    }
}

UT_NAMESPACE_END
