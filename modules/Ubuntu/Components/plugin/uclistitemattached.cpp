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

UCListItemAttached::UCListItemAttached(QObject *parent)
    : QObject(parent)
    , m_container(0)
    , m_swiping(false)
{
}

UCListItemAttached::~UCListItemAttached()
{
}

/*!
 * \qmlattachedproperty ListItemActions ListItem::container
 * \readonly
 * The property holds the instance of the \l ListItemActions the ListItem's actions
 * panel is visualizing.
 */
void UCListItemAttached::setList(UCListItemActions *list)
{
    if (list == m_container) {
        return;
    }
    m_container = list;
    if (m_container) {
        // connect statusChanged() to update status, listItem, listItemIndex and overshoot values
        QObject::connect(m_container, &UCListItemActions::statusChanged,
                         this, &UCListItemAttached::statusChanged);
        QObject::connect(m_container, &UCListItemActions::statusChanged,
                         this, &UCListItemAttached::listItemChanged);
        QObject::connect(m_container, &UCListItemActions::statusChanged,
                         this, &UCListItemAttached::listItemIndexChanged);

        UCListItemActionsPrivate *actions = UCListItemActionsPrivate::get(m_container);
        // connect panel's xChanged to update the dragged offset
        QObject::connect(actions->panelItem, &QQuickItem::xChanged,
                         this, &UCListItemAttached::offsetChanged);

        // connect actions to get updates about visible changes
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(m_container)->actions) {
            QObject::connect(action, &UCAction::visibleChanged,
                             this, &UCListItemAttached::updateVisibleActions);
        }
        updateVisibleActions();
        Q_EMIT containerChanged();
        Q_EMIT visibleActionsChanged();
    }
}

void UCListItemAttached::connectListItem(UCListItem *item, bool connect)
{
    if (!item) {
        return;
    }
    if (connect) {
        QObject::connect(item, &UCListItem::highlightedChanged,
                         this, &UCListItemAttached::updateSwipeState);
        QObject::connect(item, &UCListItem::contentMovingChanged,
                         this, &UCListItemAttached::updateSwipeState);
    } else {
        QObject::disconnect(item, &UCListItem::highlightedChanged,
                            this, &UCListItemAttached::updateSwipeState);
        QObject::disconnect(item, &UCListItem::contentMovingChanged,
                            this, &UCListItemAttached::updateSwipeState);
    }
}

// private slot to update visible actions
void UCListItemAttached::updateVisibleActions()
{
    m_visibleActions.clear();
    if (m_container) {
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(m_container)->actions) {
            if (action->m_visible) {
                m_visibleActions << action;
            }
        }
    }
    Q_EMIT visibleActionsChanged();
}

// private slot updating swipe state
void UCListItemAttached::updateSwipeState()
{
    if (!m_container) {
        return;
    }
    QQuickItem *panelItem = UCListItemActionsPrivate::get(m_container)->panelItem;
    if (!panelItem || !panelItem->parentItem()) {
        return;
    }
    UCListItem *item = static_cast<UCListItem*>(panelItem->parentItem());
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    bool swiped = listItem->highlighted && listItem->contentMoved;
    if (swiped != m_swiping) {
        m_swiping = swiped;
        Q_EMIT swipingChanged();
    }
}

/*!
 * \qmlattachedproperty list<Action> ListItem::visibleActions
 * Holds the list of visible actions. This is a convenience property to help action
 * visualization panel implementations to consider only visible actions.
 */
QQmlListProperty<UCAction> UCListItemAttached::visibleActions()
{
    return QQmlListProperty<UCAction>(this, m_visibleActions);
}

/*!
 * \qmlattachedproperty ListItem ListItem::listItem
 * \readonly
 * The property reports the connected \l ListItem to the list of actions.
 */
UCListItem *UCListItemAttached::listItem()
{
    if (!m_container) {
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
 * \qmlattachedproperty int ListItem::listItemIndex
 * \readonly
 * Holds the index of the \l ListItem within a view, if the \l ListItem is used
 * in a model driven view. Otherwise it is set to -1.
 */
int UCListItemAttached::listItemIndex() {
    if (!m_container) {
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
 * \qmlattachedproperty bool ListItem::swiping
 * \readonly
 * The property notifies whether the panel is swiped or not. The property does
 * not notify the rebounding.
 */
bool UCListItemAttached::swiping()
{
    return m_swiping;
}

/*!
 * \qmlattachedproperty real ListItem::offset
 * The property returns the offset the panel item holding the visualized actions
 * is visible. This can be used to do different animations on the panel and on
 * the action visualizations.
 */
qreal UCListItemAttached::offset()
{
    if (!m_container) {
        return 0.0;
    }
    return UCListItemActionsPrivate::get(m_container)->offsetDragged;
}

/*!
 * \qmlattachedproperty enum ListItem::status
 * \readonly
 * The property holds the status of the ListItemActions, whether is connected
 * as leading or as trailing action list to a \l ListItem. Possible valueas are:
 * \list A
 *  \li \b Disconnected - default, the actions list is not connected to any \l ListItem
 *  \li \b Leading - the actions list is connected as leading list
 *  \li \b Trailing - the actions list is connected as trailing list
 * \endlist
 */
UCListItem::Status UCListItemAttached::status()
{
    if (!m_container) {
        return UCListItem::Disconnected;
    }
    return UCListItemActionsPrivate::get(m_container)->status;
}

/*!
 * \qmlattachedmethod void ListItem::snapToPosition(real position)
 * The function can be used to perform custom snapping, or to execute rebounding
 * and also disconnecting from the connected \l ListItem. This can be achieved by
 * calling the function with 0.0 value.
 */
void UCListItemAttached::snapToPosition(qreal position)
{
    //if it is disconnected, leave (this also includes the case when m_container is null)
    if (status() == UCListItem::Disconnected) {
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
    position *= (status() == UCListItem::Leading) ? 1 : -1;
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
