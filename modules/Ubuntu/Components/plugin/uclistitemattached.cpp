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

#include "ucunits.h"
#include "uctheme.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "propertychange_p.h"
#include <QtQuick/private/qquickflickable_p.h>

UCListItemAttachedPrivate::UCListItemAttachedPrivate(UCListItemAttached *qq)
    : q_ptr(qq)
{
}

UCListItemAttachedPrivate::~UCListItemAttachedPrivate()
{
    // clear property change objects
    Q_Q(UCListItemAttached);
    q->disableInteractive(false);
    Q_FOREACH(const Record &record, list) {
        QObject::disconnect(record.flickable.data(), &QQuickFlickable::movementStarted,
                         q, 0);
        delete record.interactive;
    }
}

void UCListItemAttachedPrivate::init()
{
    // collect all ascendant flickables
    Q_Q(UCListItemAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            Record record;
            record.flickable = flickable;
            record.interactive = new PropertyChange(item, "interactive");
            list.append(record);
            QObject::connect(flickable, &QQuickFlickable::movementStarted,
                             q, &UCListItemAttached::unbindItem);
        }
        item = item->parentItem();
    }
}

UCListItemAttached::UCListItemAttached(QObject *owner)
    : QObject(owner)
    , d_ptr(new UCListItemAttachedPrivate(this))
{
    d_ptr->init();
}

UCListItemAttached::~UCListItemAttached()
{
}

// register item to be rebount
bool UCListItemAttached::listenToRebind(UCListItem *item, bool listen)
{
    // we cannot bind the item until we have an other one bount
    Q_D(UCListItemAttached);
    if (listen) {
        if (d->bountItem.isNull() || (d->bountItem == item)) {
            d->bountItem = item;
            return true;
        }
        return false;
    } else if (d->bountItem == item) {
        d->bountItem.clear();
        return true;
    }
    return false;
}

bool UCListItemAttached::isMoving()
{
    Q_D(UCListItemAttached);
    Q_FOREACH(const UCListItemAttachedPrivate::Record &record, d->list) {
        if (record.flickable && record.flickable->isMoving()) {
            return true;
        }
    }
    return false;
}

bool UCListItemAttached::isBountTo(UCListItem *item)
{
    Q_D(UCListItemAttached);
    return d->bountItem == item;
}

void UCListItemAttached::disableInteractive(bool disable)
{
    Q_D(UCListItemAttached);
    Q_FOREACH(const UCListItemAttachedPrivate::Record &record, d->list) {
        if (disable) {
            PropertyChange::setValue(record.interactive, false);
        } else {
            PropertyChange::restore(record.interactive);
        }
    }
}

void UCListItemAttached::unbindItem()
{
    Q_D(UCListItemAttached);
    if (d->bountItem) {
        // depending on content item's X coordinate, we either rebound or prompt drop
        if (d->bountItem->contentItem()->x() != 0.0) {
            // content is not in origin, rebind
            UCListItemPrivate::get(d->bountItem.data())->_q_rebound();
        } else {
            // do some cleanup
            UCListItemPrivate::get(d->bountItem.data())->promptRebound();
        }
        d->bountItem.clear();
    }
}
