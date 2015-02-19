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
    : QQuickMouseArea(parent)
    , listView(static_cast<QQuickFlickable*>(parent))
    , viewAttached(0)
    , direction(UCDragEvent::Steady)
    , scrollDirection(UCDragEvent::Steady)
    , fromIndex(-1)
    , toIndex(-1)
    , min(-1)
    , max(-1)
{
    connect(this, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(startDragging(QQuickMouseEvent*)), Qt::DirectConnection);
    connect(this, SIGNAL(released(QQuickMouseEvent*)), this, SLOT(stopDragging(QQuickMouseEvent*)), Qt::DirectConnection);
    connect(this, SIGNAL(positionChanged(QQuickMouseEvent*)), this, SLOT(updateDragging(QQuickMouseEvent*)), Qt::DirectConnection);

    // for testing purposes
    setObjectName("drag_area");
}

void ListItemDragArea::init(const QRectF &area)
{
    setParentItem(static_cast<QQuickItem*>(parent()));
    updateArea(area);
    reset();

    // warn if no draggingUpdated() signal handler is implemented
    viewAttached = static_cast<UCViewItemsAttached*>(
                qmlAttachedPropertiesObject<UCViewItemsAttached>(listView));
    if (viewAttached && !UCViewItemsAttachedPrivate::get(viewAttached)->isDraggingUpdatedConnected()) {
        qmlInfo(parentItem()) << UbuntuI18n::instance().tr(
                                     "ListView has no ViewItems.draggingUpdated() signal handler implemented. "\
                                     "No dragging will be possible");
    }
}

void ListItemDragArea::updateArea(const QRectF &area)
{
    QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
    anchors->setFill(listView);
    // set the margins based on the area
    anchors->setLeftMargin(area.left());
    if (area.right() != 0.0) {
        anchors->setRightMargin(listView->width() - area.right());
    }
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
    QQuickMouseArea::timerEvent(event);
    if (event->timerId() == scrollTimer.timerId()) {
        qreal scrollAmount = UCUnits::instance().gu(0.5) * (scrollDirection == UCDragEvent::Upwards ? -1 : 1);
        qreal contentHeight = listView->contentHeight();
        qreal height = listView->height();
        if ((contentHeight - height) > 0) {
            // take topMargin into account when clamping
            qreal contentY = CLAMP(listView->contentY() + scrollAmount,
                                   -listView->topMargin(),
                                   contentHeight - height + listView->originY());
            listView->setContentY(contentY);
            // update
            updateDragging(0);
        }
    }
}

// starts dragging operation; emits draggingStarted() and if the signal handler is implemented,
// depending on the acceptance, will create a fake item and will start dragging. If the start is
// cancelled, no dragging will happen
void ListItemDragArea::startDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    QPointF pos = mapDragAreaPos();
    UCListItem *listItem = itemAt(pos.x(), pos.y());
    if (!listItem) {
        return;
    }
    int index = indexAt(pos.x(), pos.y());
    bool start = true;
    max = min = -1;
    // call start handler if implemented
    UCViewItemsAttachedPrivate *pViewAttached = UCViewItemsAttachedPrivate::get(viewAttached);
    if (pViewAttached->isDraggingStartedConnected()) {
        UCDragEvent drag(UCDragEvent::Steady, index, -1, -1, -1);
        Q_EMIT viewAttached->draggingStarted(&drag);
        start = drag.m_accept;
        min = drag.m_minimum;
        max = drag.m_maximum;
    }
    if (start) {
        pViewAttached->buildChangesList(false);
        fromIndex = toIndex = index;
        lastPos = pos;
        // create temp drag item
        createDraggedItem(listItem);
    }
}

// stops dragging, performs drop event (event.direction = ListItemDrag.Steady)
// and clears temporary item
void ListItemDragArea::stopDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
    if (item.isNull()) {
        return;
    }
    // stop scroll timer
    scrollTimer.stop();
    UCViewItemsAttachedPrivate *pViewAttached = UCViewItemsAttachedPrivate::get(viewAttached);
    if (pViewAttached->isDraggingUpdatedConnected() && (fromIndex != toIndex)) {
        UCDragEvent drag(UCDragEvent::Steady, fromIndex, toIndex, min, max);
        Q_EMIT viewAttached->draggingUpdated(&drag);
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

void ListItemDragArea::updateDragging(QQuickMouseEvent *event)
{
    Q_UNUSED(event);
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
    scrollDirection = UCDragEvent::Steady;
    if (topHotspot < topViewMargin) {
        // scroll upwards
        scrollDirection = UCDragEvent::Upwards;
    } else if (bottomHotspot > bottomViewMargin) {
        // scroll downwards
        scrollDirection = UCDragEvent::Downwards;
    }
    if (scrollDirection == UCDragEvent::Steady) {
        scrollTimer.stop();
    } else if (!scrollTimer.isActive()){
        scrollTimer.start(DRAG_SCROLL_TIMEOUT, this);
    }

    // do we have index change?
    if (toIndex == index) {
        // no change, leave
        return;
    }
    // update drag direction to make sure we're reflecting the proper one
    direction = (toIndex > index) ? UCDragEvent::Upwards : UCDragEvent::Downwards;

    toIndex = index;
    if (fromIndex != toIndex) {
        bool update = true;
        UCViewItemsAttachedPrivate *pViewAttached = UCViewItemsAttachedPrivate::get(viewAttached);
        if (pViewAttached->isDraggingUpdatedConnected()) {
            UCDragEvent drag(direction, fromIndex, toIndex, min, max);
            Q_EMIT viewAttached->draggingUpdated(&drag);
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
    QPointF pos(mouseX(), mouseY() + listView->contentY());
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
