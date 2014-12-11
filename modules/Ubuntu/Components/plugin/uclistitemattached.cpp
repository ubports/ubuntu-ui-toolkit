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
#include "quickutils.h"
#include "i18n.h"
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQml/QQmlInfo>

/*
 * The properties are attached to the ListItem's parent item or to its closest
 * Flickable parent, when embedded in ListView or Flickable. There will be only
 * one attached property per Flickable for all embedded child ListItems, enabling
 * in this way the controlling of the interactive flag of the Flickable and all
 * its ascendant Flickables.
 */
UCListItemAttachedPrivate::UCListItemAttachedPrivate(UCListItemAttached *qq)
    : q_ptr(qq)
    , listView(0)
    , globalDisabled(false)
    , selectable(false)
    , draggable(false)
{
}

UCListItemAttachedPrivate::~UCListItemAttachedPrivate()
{
    clearChangesList();
    clearFlickablesList();
}

// disconnect all flickables
void UCListItemAttachedPrivate::clearFlickablesList()
{
    Q_Q(UCListItemAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, flickables) {
        if (flickable.data())
        QObject::disconnect(flickable.data(), &QQuickFlickable::movementStarted,
                            q, &UCListItemAttached::unbindItem);
    }
    flickables.clear();
}

// connect all flickables
void UCListItemAttachedPrivate::buildFlickablesList()
{
    Q_Q(UCListItemAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearFlickablesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            QObject::connect(flickable, &QQuickFlickable::movementStarted,
                             q, &UCListItemAttached::unbindItem);
            flickables << flickable;
        }
        item = item->parentItem();
    }
}

void UCListItemAttachedPrivate::clearChangesList()
{
    // clear property change objects
    Q_Q(UCListItemAttached);
    Q_FOREACH(PropertyChange *change, changes) {
        // deleting PropertyChange will restore the saved property
        // to its original binding/value
        delete change;
    }
    changes.clear();
}

void UCListItemAttachedPrivate::buildChangesList(const QVariant &newValue)
{
    // collect all ascendant flickables
    Q_Q(UCListItemAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearChangesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            PropertyChange *change = new PropertyChange(item, "interactive");
            PropertyChange::setValue(change, newValue);
            changes << change;
        }
        item = item->parentItem();
    }
}

UCListItemAttached::UCListItemAttached(QObject *owner)
    : QObject(owner)
    , d_ptr(new UCListItemAttachedPrivate(this))
{
    if (QuickUtils::inherits(owner, "QQuickListView")) {
        d_ptr->listView = static_cast<QQuickFlickable*>(owner);
    }
}

UCListItemAttached::~UCListItemAttached()
{
}

// register item to be rebound
bool UCListItemAttached::listenToRebind(UCListItem *item, bool listen)
{
    // we cannot bind the item until we have an other one bound
    bool result = false;
    Q_D(UCListItemAttached);
    if (listen) {
        if (d->boundItem.isNull() || (d->boundItem == item)) {
            d->boundItem = item;
            // rebuild flickable list
            d->buildFlickablesList();
            result = true;
        }
    } else if (d->boundItem == item) {
        d->boundItem.clear();
        result = true;
    }
    return result;
}

// reports true if any of the ascendant flickables is moving
bool UCListItemAttached::isMoving()
{
    Q_D(UCListItemAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, d->flickables) {
        if (flickable && flickable->isMoving()) {
            return true;
        }
    }
    return false;
}

// returns true if the given ListItem is bound to listen on moving changes
bool UCListItemAttached::isBoundTo(UCListItem *item)
{
    Q_D(UCListItemAttached);
    return d->boundItem == item;
}

/*
 * Disable/enable interactive flag for the ascendant flickables. The item is used
 * to detect whether the same item is trying to enable the flickables which disabled
 * it before. The enabled/disabled states are not equivalent to the enabled/disabled
 * state of the interactive flag.
 * When disabled, always the last item disabling will be kept as active disabler,
 * and only the active disabler can enable (restore) the interactive flag state.
 */
void UCListItemAttached::disableInteractive(UCListItem *item, bool disable)
{
    Q_D(UCListItemAttached);
    if (disable) {
        // disabling or re-disabling
        d->disablerItem = item;
        if (d->globalDisabled == disable) {
            // was already disabled, leave
            return;
        }
        d->globalDisabled = true;
    } else if (d->globalDisabled && d->disablerItem == item) {
        // the one disabled it will enable
        d->globalDisabled = false;
        d->disablerItem.clear();
    } else {
        // !disabled && (!globalDisabled || item != d->disablerItem)
        return;
    }
    if (disable) {
        // (re)build changes list with disabling the interactive value
        d->buildChangesList(false);
    } else {
        d->clearChangesList();
    }
}

void UCListItemAttached::unbindItem()
{
    Q_D(UCListItemAttached);
    if (d->boundItem) {
        // depending on content item's X coordinate, we either do animated or prompt rebind
        if (d->boundItem->contentItem()->x() != 0.0) {
            // content is not in origin, rebind
            UCListItemPrivate::get(d->boundItem.data())->_q_rebound();
        } else {
            // do some cleanup
            UCListItemPrivate::get(d->boundItem.data())->promptRebound();
        }
        d->boundItem.clear();
    }
    // clear binding list
    d->clearFlickablesList();
}

/*!
 * \qmlattachedproperty bool ListItem::selectMode
 * The property drives whether list items are selectable or not. The property is
 * attached to the ListItem's parent or to the ListView/Flickable owning the
 * ListItems.
 *
 *
 * When set, the items
 * will show a check box on the leading side hanving the content item pushed towards
 * trailing side and dimmed. The checkbox which will reflect and drive the \l selected
 * state.
 * Defaults to false.
 */
bool UCListItemAttachedPrivate::selectMode() const
{
    return selectable;
}
void UCListItemAttachedPrivate::setSelectMode(bool value)
{
    if (selectable == value) {
        return;
    }
    selectable = value;
    Q_Q(UCListItemAttached);
    Q_EMIT q->selectModeChanged();
}

/*!
 * \qmlattachedproperty list<int> ListItem::selectedIndexes
 * The property is automatically attached to the ListItem's parent item, or to
 * the ListView when used with ListView. Contains the indexes of the ListItems
 * marked as selected. The indexes are model indexes when used in ListView, and
 * child indexes in other contexts.
 * \note Setting the ListItem's \l selected property to \c true will add the
 * item index to the selection list automatically, and may destroy the initial
 * state of the selection. Therefore it is recommended to drive the selection
 * through the attached property rather through the \l ListItem::selected property.
 * \sa ListItem::selectable, ListItem::selected
 */
QList<int> UCListItemAttachedPrivate::selectedIndexes() const
{
    return selectedList;
}
void UCListItemAttachedPrivate::setSelectedIndexes(const QList<int> &list)
{
    if (selectedList == list) {
        return;
    }
    selectedList = list;
    Q_Q(UCListItemAttached);
    Q_EMIT q->selectedIndexesChanged();
}

void UCListItemAttachedPrivate::addSelectedItem(UCListItem *item)
{
    int index = UCListItemPrivate::get(item)->index();
    if (!selectedList.contains(index)) {
        selectedList.append(index);
        Q_EMIT q_ptr->selectedIndexesChanged();
    }
}
void UCListItemAttachedPrivate::removeSelectedItem(UCListItem *item)
{
    if (selectedList.removeAll(UCListItemPrivate::get(item)->index()) > 0) {
        Q_EMIT q_ptr->selectedIndexesChanged();
    }
}

bool UCListItemAttachedPrivate::isItemSelected(UCListItem *item)
{
    return selectedList.contains(UCListItemPrivate::get(item)->index());
}

/*!
 * \qmlattachedproperty bool ListItem::dragMode
 * The property drives the dragging mode of the ListItems within a ListView. It
 * has no effect on any other parent of the ListItem.
 *
 * When set, ListItem content will be disabled and a panel will be shown enabling
 * the dragging mode. The items can be dragged by dragging this handler only.
 * The feature can be activated same time with \l selectable.
 *
 * The panel is configured by the \l {ListItemStyle::dragHandlerDelegate}{dragHandlerDelegate}
 * component.
 *
 * \sa ListItemStyle::dragHandlerDelegate, draggingStarted, draggingEnded
 */

/*!
 * \qmlattachedsignal ListItem::draggingStarted(DragEvent drag)
 * The signal is emitted when a ListItem dtragging is started. \c drag.from
 * specifies the index of the ListItem being dragged. \c drag.direction specifies
 * the direction the drag is started, which can be denied by setting \c false
 * to \c drag.accept property. If denied, the signal will be emitted again if
 * dragged into opposite direction.
 */

/*!
 * \qmlattachedsignal ListItem::draggingUpdated(DragEvent drag)
 * The signal is emitted when the list item from \c drag.from index has been
 * dragged over to \c drag.to, and a move operation is possible. Implementations
 * must move the model data between these indexes. If the move is not acceptable,
 * it can be dropped by setting \c drag.accept to \c false, in which case the
 * dragged item will stay on its last moved position or will snap back to its
 * previous place. If the direction is not suitable, it can be denied by setting
 * \c false to \c drag.accept property.
 */

bool UCListItemAttachedPrivate::dragMode() const
{
    return draggable;
}
void UCListItemAttachedPrivate::setDragMode(bool value)
{
    if (draggable == value) {
        return;
    }
    Q_Q(UCListItemAttached);
    if (value) {
        /*
         * The dragging works only if the ListItem is used inside a ListView, and the
         * model used is a list, a ListModel or a derivate of QAbstractItemModel. Do
         * not enable dragging if these conditions are not fulfilled.
         */
        if (!listView) {
            qmlInfo(q->parent()) << UbuntuI18n::instance().tr("dragging mode requires ListView");
            return;
        }
        QVariant modelValue = listView->property("model");
        if (!modelValue.isValid()) {
            return;
        }
        if (modelValue.type() == QVariant::Int || modelValue.type() == QVariant::Double) {
            qmlInfo(listView) << UbuntuI18n::instance().tr("model must be a list, ListModel or a derivate of QAbstractItemModel");
            return;
        }
    }
    draggable = value;
    if (draggable) {
        listView->installEventFilter(q);
    } else {
        listView->removeEventFilter(q);
    }
    Q_EMIT q->dragModeChanged();
}

bool UCListItemAttached::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(UCListItemAttached);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    {
        d->mousePressed(d->listView, static_cast<QMouseEvent*>(event));
        break;
    }
    case QEvent::MouseButtonRelease:
    {
        d->mouseReleased(d->listView, static_cast<QMouseEvent*>(event));
        break;
    }
    case QEvent::MouseMove:
    {
        d->mouseMoved(d->listView, static_cast<QMouseEvent*>(event));
        break;
    }
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

QQuickItem *UCListItemAttachedPrivate::lastChildAt(QQuickItem *parent, QPointF pos)
{
    QQuickItem *child = parent->childAt(pos.x(), pos.y());
    while (child) {
        pos = child->mapFromItem(parent, pos);
        // find the next one
        QQuickItem *next = child->childAt(pos.x(), pos.y());

        // are we in a ListItem already?
        UCListItem *listItem = qobject_cast<UCListItem*>(child);
        if (listItem && UCListItemPrivate::get(listItem)->dragHandler->isPanel(next)) {
            // we stop here and check if the press occurred over drag handler's panel
            child = next;
            break;
        } else if (next) {
            parent = child;
            child = next;
        } else {
            break;
        }
    }
    return child;
}

void UCListItemAttachedPrivate::mousePressed(QQuickItem *sender, QMouseEvent *event)
{
    QQuickItem *child = lastChildAt(sender, event->localPos());
    qDebug() << "PRESSED OVER" << child;
}

void UCListItemAttachedPrivate::mouseReleased(QQuickItem *sender, QMouseEvent *event)
{
    Q_UNUSED(event);
    QQuickItem *child = lastChildAt(sender, event->localPos());
    qDebug() << "RELEASED OVER" << child;
}

void UCListItemAttachedPrivate::mouseMoved(QQuickItem *sender, QMouseEvent *event)
{
    Q_UNUSED(event);
    QQuickItem *child = lastChildAt(sender, event->localPos());
    qDebug() << "MOVED OVER" << child;
}
