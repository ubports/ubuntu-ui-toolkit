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

UCListItemActionsAttached::UCListItemActionsAttached(QObject *parent)
    : QObject(parent)
    , m_dragging(false)
{
}

UCListItemActionsAttached::~UCListItemActionsAttached()
{
}


/*!
 * \qmlattachedproperty ListItemActions ListItemActions::container
 * \readonly
 * The property holds the instance of the ListItemActions the \l panelItem is
 * attached to.
 */
void UCListItemActionsAttached::setList(UCListItemActions *list)
{
    if (list == m_container) {
        return;
    }
    m_container = list;
    if (!m_container.isNull()) {
        // connect to get status updates, listItem and index changes
        connect(m_container.data(), &UCListItemActions::statusChanged,
                this, &UCListItemActionsAttached::statusChanged);
        connect(m_container.data(), &UCListItemActions::statusChanged,
                this, &UCListItemActionsAttached::listItemChanged);
        connect(m_container.data(), &UCListItemActions::statusChanged,
                this, &UCListItemActionsAttached::listItemIndexChanged);
    }
    Q_EMIT containerChanged();
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
 * in a modelled view. Otherwise it is set to -1.
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
    return listItem ? UCListItemPrivate::get(static_cast<UCListItem*>(listItem))->index : -1;
}

/*!
 * \qmlattachedproperty bool ListItemActions::dragging
 * \readonly
 * The property notifies whether the panel is dragged or not. The property does
 * not notify the rebounding.
 */
void UCListItemActionsAttached::setDrag(bool value)
{
    if (value == m_dragging) {
        return;
    }
    m_dragging = value;
    Q_EMIT draggingChanged();
}

/*!
 * \qmlattachedproperty real ListItemActions::offsetVisible
 * The property returns the offset the \l panelItem is visible. This can be used
 * to do different animations on the panel and on the action visualizations.
 */
qreal UCListItemActionsAttached::offsetVisible()
{
    if (m_container.isNull()) {
        return 0.0;
    }
    return UCListItemActionsPrivate::get(m_container)->offsetDragged;
}

/*!
 * \qmlattachedproperty enum ListItemActions::status
 * \readonly
 * This is the proxied \c status property of \l ListItemActions.
 */
UCListItemActions::Status UCListItemActionsAttached::status()
{
    if (m_container.isNull()) {
        return UCListItemActions::Disconnected;
    }
    return UCListItemActionsPrivate::get(m_container)->status;
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
    QQuickItem *panelItem = m_container->panelItem();
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
    position *= (m_container->status() == UCListItemActions::Leading) ? 1 : -1;
    if (position == 0.0) {
        listItem->_q_rebound();
    } else {
        listItem->reboundTo(position);
    }
}
