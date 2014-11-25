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

#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "ucunits.h"
#include "ucaction.h"

UCListItemActionsAttached::UCListItemActionsAttached(QObject *parent)
    : QObject(parent)
    , m_swiping(false)
{
}

UCListItemActionsAttached::~UCListItemActionsAttached()
{
}

/*!
 * \qmlattachedproperty ListItemActions ListItemActions::container
 * \readonly
 * The property holds the instance of the \l ListItemActions the ListItem's actions
 * panel is visualizing.
 */
void UCListItemActionsAttached::setList(UCListItemActions *list)
{
    if (list == m_container) {
        return;
    }
    m_container = list;
    if (!m_container.isNull()) {
        // connect statusChanged() to update status, listItem, listItemIndex and overshoot values
        QObject::connect(m_container.data(), &UCListItemActions::statusChanged,
                         this, &UCListItemActionsAttached::statusChanged);
        QObject::connect(m_container.data(), &UCListItemActions::statusChanged,
                         this, &UCListItemActionsAttached::listItemChanged);
        QObject::connect(m_container.data(), &UCListItemActions::statusChanged,
                         this, &UCListItemActionsAttached::listItemIndexChanged);
        QObject::connect(m_container.data(), &UCListItemActions::statusChanged,
                         this, &UCListItemActionsAttached::overshootChanged);

        UCListItemActionsPrivate *actions = UCListItemActionsPrivate::get(m_container.data());
        // connect panel's xChanged to update the dragged offset
        QObject::connect(actions->panelItem, &QQuickItem::xChanged,
                         this, &UCListItemActionsAttached::offsetChanged);

        // connect actions to get updates about visible changes
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(m_container)->actions) {
            QObject::connect(action, &UCAction::visibleChanged,
                             this, &UCListItemActionsAttached::updateVisibleActions);
        }
        updateVisibleActions();
    }
    Q_EMIT containerChanged();
    Q_EMIT visibleActionsChanged();
}

void UCListItemActionsAttached::connectListItem(UCListItem *item, bool connect)
{
    if (!item) {
        return;
    }
    if (connect) {
        QObject::connect(item, &UCListItem::pressedChanged,
                         this, &UCListItemActionsAttached::updateSwipeState);
        QObject::connect(item, &UCListItem::contentMovingChanged,
                         this, &UCListItemActionsAttached::updateSwipeState);
    } else {
        QObject::disconnect(item, &UCListItem::pressedChanged,
                            this, &UCListItemActionsAttached::updateSwipeState);
        QObject::disconnect(item, &UCListItem::contentMovingChanged,
                            this, &UCListItemActionsAttached::updateSwipeState);
    }
}

// private slot to update visible actions
void UCListItemActionsAttached::updateVisibleActions()
{
    m_visibleActions.clear();
    if (!m_container.isNull()) {
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(m_container)->actions) {
            if (action->m_visible) {
                m_visibleActions << action;
            }
        }
    }
    Q_EMIT visibleActionsChanged();
}

// private slot updating swipe state
void UCListItemActionsAttached::updateSwipeState()
{
    if (m_container.isNull()) {
        return;
    }
    QQuickItem *panelItem = UCListItemActionsPrivate::get(m_container)->panelItem;
    if (!panelItem || !panelItem->parentItem()) {
        return;
    }
    UCListItem *item = static_cast<UCListItem*>(panelItem->parentItem());
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    bool swiped = listItem->pressed && listItem->contentMoved;
    if (swiped != m_swiping) {
        m_swiping = swiped;
        Q_EMIT swipingChanged();
    }
}

/*!
 * \qmlattachedproperty list<Action> ListItemActions::visibleActions
 * Holds the list of visible actions. This is a convenience property to help action
 * visualization panel implementations to consider only visible actions.
 */
QQmlListProperty<UCAction> UCListItemActionsAttached::visibleActions()
{
    return QQmlListProperty<UCAction>(this, m_visibleActions);
}

/*!
 * \qmlattachedproperty ListItem ListItemActions::listItem
 * \readonly
 * The property reports the connected \l ListItem to the list of actions.
 */
UCListItem *UCListItemActionsAttached::listItem()
{
    if (m_container.isNull()) {
        return NULL;
    }
    QQuickItem *panelItem = UCListItemActionsPrivate::get(m_container)->panelItem;
    if (!panelItem) {
        return NULL;
    }
    QQuickItem *listItem = panelItem->parentItem();
    return static_cast<UCListItem*>(listItem);
}

/*!
 * \qmlattachedproperty int ListItemActions::listItemIndex
 * \readonly
 * Holds the index of the \l ListItem within a view, if the \l ListItem is used
 * in a model driven view. Otherwise it is set to -1.
 */
int UCListItemActionsAttached::listItemIndex() {
    if (m_container.isNull()) {
        return -1;
    }
    QQuickItem *panelItem = UCListItemActionsPrivate::get(m_container)->panelItem;
    if (!panelItem) {
        return -1;
    }
    QQuickItem *listItem = panelItem->parentItem();
    return listItem ? UCListItemPrivate::get(static_cast<UCListItem*>(listItem))->index() : -1;
}

/*!
 * \qmlattachedproperty bool ListItemActions::swiping
 * \readonly
 * The property notifies whether the panel is swiped or not. The property does
 * not notify the rebounding.
 */
bool UCListItemActionsAttached::swiping()
{
    return m_swiping;
}

/*!
 * \qmlattachedproperty real ListItemActions::offset
 * The property returns the offset the panel item holding the visualized actions
 * is visible. This can be used to do different animations on the panel and on
 * the action visualizations.
 */
qreal UCListItemActionsAttached::offset()
{
    if (m_container.isNull()) {
        return 0.0;
    }
    return UCListItemActionsPrivate::get(m_container)->offsetDragged;
}

/*!
 * \qmlattachedproperty enum ListItemActions::status
 * \readonly
 * The property holds the status of the ListItemActions, whether is connected
 * as leading or as trailing action list to a \l ListItem. Possible valueas are:
 * \list A
 *  \li \b Disconnected - default, the actions list is not connected to any \l ListItem
 *  \li \b Leading - the actions list is connected as leading list
 *  \li \b Trailing - the actions list is connected as trailing list
 * \endlist
 */
UCListItemActions::Status UCListItemActionsAttached::status()
{
    if (m_container.isNull()) {
        return UCListItemActions::Disconnected;
    }
    return UCListItemActionsPrivate::get(m_container)->status;
}

/*!
 * \qmlattachedproperty real ListItemActions::overshoot
 * The property holds the overshoot value set for the list item.
 */
qreal UCListItemActionsAttached::overshoot()
{
    if (status() == UCListItemActions::Disconnected) {
        return 0.0;
    }
    QQuickItem *panelItem = UCListItemActionsPrivate::get(m_container)->panelItem;
    if (!panelItem) {
        // we don't have the panel created yet
        return 0.0;
    }
    UCListItem *item = static_cast<UCListItem*>(panelItem->parentItem());
    if (!item) {
        // no ListItem attached
        return 0.0;
    }
    return UCListItemPrivate::get(item)->overshoot;
}

/*!
 * \qmlattachedmethod void ListItemActions::snapToPosition(real position)
 * The function can be used to perform custom snapping, or to execute rebounding
 * and also disconnecting from the connected \l ListItem. This can be achieved by
 * calling the function with 0.0 value.
 */
void UCListItemActionsAttached::snapToPosition(qreal position)
{
    //if it is disconnected, leave (this also includes the case when m_container is null)
    if (status() == UCListItemActions::Disconnected) {
        return;
    }
    QQuickItem *panelItem = UCListItemActionsPrivate::get(m_container)->panelItem;
    if (!panelItem) {
        // we don't have the panel created yet
        return;
    }
    UCListItem *item = static_cast<UCListItem*>(panelItem->parentItem());
    if (!item) {
        // no ListItem attached
        return;
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    position *= (status() == UCListItemActions::Leading) ? 1 : -1;
    if (position == 0.0) {
        listItem->_q_rebound();
    } else {
        if (listItem->animator) {
            listItem->animator->snap(position);
        } else {
            listItem->contentItem->setX(position);
        }
    }
}


