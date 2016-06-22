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

#ifndef LISTITEMDRAGHANDLER_H
#define LISTITEMDRAGHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QPointF>
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UCListItem;
class PropertyChange;
class ListItemDragHandler : public QObject
{
    Q_OBJECT
public:
    explicit ListItemDragHandler(UCListItem *baseItem, UCListItem *listItem = 0);
    ~ListItemDragHandler();

    void init();
    void drop();
    void update(UCListItem *hoveredItem);

private Q_SLOTS:
    void dropItem();

private:
    UCListItem *listItem;
    QPointer<UCListItem> baseItem;
    PropertyChange *baseVisible;
    QPointF targetPos;
};

UT_NAMESPACE_END

#endif // LISTITEMDRAGHANDLER_H
