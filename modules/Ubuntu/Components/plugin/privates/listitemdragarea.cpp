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

#include "listitemdragarea.h"
#include "listitemdraghandler.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "ucunits.h"
#include "i18n.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>

#define IMPLICIT_DRAG_WIDTH_GU  5
#define DRAG_SCROLL_TIMEOUT     15

#define MIN(x, y)           ((x) < (y) ? (x) : (y))
#define MAX(x, y)           ((x) > (y) ? (x) : (y))
#define CLAMP(v, min, max)  ((min) <= (max) ? MAX(min, MIN(v, max)) : MAX(max, MIN(v, min)))

ListItemDragArea::ListItemDragArea(QQuickItem *parent)
    : QQuickItem(parent)
    , listView(static_cast<QQuickFlickable*>(parent))
    , viewAttached(0)
    , scrollDirection(0)
    , fromIndex(-1)
    , toIndex(-1)
    , min(-1)
    , max(-1)
{
    setAcceptedMouseButtons(Qt::LeftButton);

    // for testing purposes
    setObjectName("drag_area");
}

void ListItemDragArea::init()
{
    setParentItem(static_cast<QQuickItem*>(parent()));
    QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
    anchors->setFill(parentItem());
    viewAttached = static_cast<UCViewItemsAttached*>(
                qmlAttachedPropertiesObject<UCViewItemsAttached>(listView));
    reset();
}

void ListItemDragArea::reset()
{
    fromIndex = toIndex = min = max = -1;
    item = 0;
    lastPos = QPointF();
    setEnabled(true);
}

void ListItemDragArea::timerEvent(QTimerEvent *event)
{
    QQuickItem::timerEvent(event);
    if (event->timerId() == scrollTimer.timerId()) {
        qreal scrollAmount = UCUnits::instance().gu(0.5) * scrollDirection;
        qreal contentHeight = listView->contentHeight();
        qreal height = listView->height();
        if ((contentHeight - height) > 0) {
            // take topMargin into account when clamping
            qreal contentY = CLAMP(listView->contentY() + scrollAmount,
                                   -listView->topMargin(),
                                   contentHeight - height + listView->originY());
            listView->setContentY(contentY);
            // update
            mouseMoveEvent(0);
        }
    }
}

// starts dragging operation; emits draggingStarted() and if the signal handler is implemented,
// depending on the acceptance, will create a fake item and will start dragging. If the start is
// cancelled, no dragging will happen
void ListItemDragArea::mousePressEvent(QMouseEvent *event)
{
    mousePos = event->localPos();
    QPointF pos = mapDragAreaPos();
    UCListItem *listItem = itemAt(pos.x(), pos.y());
    if (!listItem) {
        event->setAccepted(false);
        return;
    }
    // check if we tapped over the drag panel
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    if (pListItem->styleItem && pListItem->listItemStyle()->m_dragPanel) {
        // convert mouse into local panel coordinates
        QPointF panelPos = pListItem->listItemStyle()->m_dragPanel->mapFromItem(this, mousePos);
        if (!pListItem->listItemStyle()->m_dragPanel->contains(panelPos)) {
            // not tapped over the drag panel, leave
            event->setAccepted(false);
            return;
        }
    }
    int index = indexAt(pos.x(), pos.y());
    bool start = false;
    max = min = -1;
    // call start handler if implemented
    UCViewItemsAttachedPrivate *pViewAttached = UCViewItemsAttachedPrivate::get(viewAttached);
    if (pViewAttached->isDragUpdatedConnected()) {
        UCDragEvent drag(UCDragEvent::Started, index, -1, -1, -1);
        Q_EMIT viewAttached->dragUpdated(&drag);
        start = drag.m_accept;
        min = drag.m_minimum;
        max = drag.m_maximum;
    } else {
        qmlInfo(parentItem()) << UbuntuI18n::instance().tr(
                                     "ListView has no ViewItems.dragUpdated() signal handler implemented. "\
                                     "No dragging will be possible.");
    }
    if (start) {
        pViewAttached->buildChangesList(false);
        fromIndex = toIndex = index;
        lastPos = pos;
        // create temp drag item
        createDraggedItem(listItem);
    }
    event->setAccepted(start);
}

// stops dragging, performs drop event (event.direction = ListItemDrag.Steady)
// and clears temporary item
void ListItemDragArea::mouseReleaseEvent(QMouseEvent *event)
{
    mousePos = event->localPos();
    if (item.isNull()) {
        return;
    }
    // stop scroll timer
    scrollTimer.stop();
    UCViewItemsAttachedPrivate *pViewAttached = UCViewItemsAttachedPrivate::get(viewAttached);
    if (pViewAttached->isDragUpdatedConnected() && (fromIndex != toIndex)) {
        UCDragEvent drag(UCDragEvent::Dropped, fromIndex, toIndex, min, max);
        Q_EMIT viewAttached->dragUpdated(&drag);
        updateDraggedItem();
        if (drag.m_accept) {
            pViewAttached->updateSelectedIndices(fromIndex, toIndex);
        }
    }
    // unlock flickables
    pViewAttached->clearChangesList();
    // perform drop
    UCListItemPrivate::get(item.data())->dragHandler->drop();
    item = 0;
    fromIndex = toIndex = -1;
}

void ListItemDragArea::mouseMoveEvent(QMouseEvent *event)
{
    if (event) {
        mousePos = event->localPos();
    }
    if (!item) {
        return;
    }
    QPointF pos = mapDragAreaPos();
    qreal dy = -(lastPos.y() - pos.y());
    qreal indexHotspot = item->y() + item->height() / 2;
    // update dragged item even if the dragging may be forbidden
    item->setY(item->y() + dy);
    lastPos = pos;

    // check what will be the index after the drag
    int index = indexAt(pos.x(), indexHotspot);
    if (index < 0) {
        return;
    }
    if ((min >= 0) && (min > index)) {
        // about to drag beyond the minimum, leave
        return;
    }
    if ((max >= 0) && (max < index)) {
        // about to drag beyond maximum, leave
        return;
    }

    // should we scroll the view? use a margin of 20% of the dragged item's height from top and bottom of the item
    qreal scrollMargin = item->height() * 0.2;
    qreal topHotspot = item->y() + scrollMargin - listView->contentY();
    qreal bottomHotspot = item->y() + item->height() - scrollMargin - listView->contentY();
    // use MouseArea's top/bottom as limits
    qreal topViewMargin = y() + listView->topMargin();
    qreal bottomViewMargin = y() + height() - listView->bottomMargin();
    scrollDirection = 0;
    if (topHotspot < topViewMargin) {
        // scroll upwards
        scrollDirection = -1;
    } else if (bottomHotspot > bottomViewMargin) {
        // scroll downwards
        scrollDirection = 1;
    }
    if (!scrollDirection) {
        scrollTimer.stop();
    } else if (!scrollTimer.isActive()){
        scrollTimer.start(DRAG_SCROLL_TIMEOUT, this);
    }

    // do we have index change?
    if (toIndex == index) {
        // no change, leave
        return;
    }

    toIndex = index;
    if (fromIndex != toIndex) {
        bool update = true;
        UCViewItemsAttachedPrivate *pViewAttached = UCViewItemsAttachedPrivate::get(viewAttached);
        if (pViewAttached->isDragUpdatedConnected()) {
            UCDragEvent drag(UCDragEvent::Moving, fromIndex, toIndex, min, max);
            Q_EMIT viewAttached->dragUpdated(&drag);
            update = drag.m_accept;
            if (update) {
                pViewAttached->updateSelectedIndices(fromIndex, toIndex);
            }
        }
        if (update) {
            // update item coordinates in the dragged item
            updateDraggedItem();
            fromIndex = toIndex;
        }
    }
}

// returns the mapped mouse position of the dragged item's dragHandler to the ListView
QPointF ListItemDragArea::mapDragAreaPos()
{
    QPointF pos(mousePos.x(), mousePos.y() + listView->contentY());
    pos = listView->mapFromItem(this, pos);
    return pos;
}

// calls ListView.indexAt() invokable
int ListItemDragArea::indexAt(qreal x, qreal y)
{
    if (!listView) {
        return -1;
    }
    int result = -1;
    QMetaObject::invokeMethod(listView, "indexAt", Qt::DirectConnection,
                              Q_RETURN_ARG(int, result),
                              Q_ARG(qreal, x),
                              Q_ARG(qreal, y)
                              );
    return result;
}

// calls ListView.itemAt() invokable
UCListItem *ListItemDragArea::itemAt(qreal x, qreal y)
{
    if (!listView) {
        return NULL;
    }
    QQuickItem *result = 0;
    QMetaObject::invokeMethod(listView, "itemAt", Qt::DirectConnection,
                              Q_RETURN_ARG(QQuickItem*, result),
                              Q_ARG(qreal, x),
                              Q_ARG(qreal, y)
                              );
    return static_cast<UCListItem*>(result);
}

// creates a temporary list item available for the dragging time
void ListItemDragArea::createDraggedItem(UCListItem *baseItem)
{
    if (item || !baseItem) {
        return;
    }
    QQmlComponent *delegate = listView->property("delegate").value<QQmlComponent*>();
    if (!delegate) {
        return;
    }
    // use baseItem's context to get access to the ListView's model roles
    // use two-step component creation to have similar steps as when it is created in QML,
    // so itemChanged() is invoked prior to componentComplete()
    // use dragged item's context as parent context so we get all model roles and
    // context properties of that item
    QQmlContext *context = new QQmlContext(qmlContext(baseItem), baseItem);
    item = static_cast<UCListItem*>(delegate->beginCreate(context));
    if (item) {
        QQml_setParent_noEvent(item, listView->contentItem());
        // create drag handler instance
        UCListItemPrivate *pItem = UCListItemPrivate::get(item);
        pItem->dragHandler = new ListItemDragHandler(baseItem, item);
        pItem->dragHandler->init();
        // invokes itemChanged()
        item->setParentItem(listView->contentItem());
        // invoked componentComplete()
        delegate->completeCreate();
    }
}

void ListItemDragArea::updateDraggedItem()
{
    if (abs(fromIndex - toIndex) > 0) {
        UCListItem *targetItem = itemAt(item->x(), item->y() + item->height() / 2);
        UCListItemPrivate::get(item)->dragHandler->update(targetItem);
    }
}
