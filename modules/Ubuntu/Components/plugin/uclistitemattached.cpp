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
    : QObject(*(new UCListItemAttachedPrivate()), parent)
{
}

UCListItemAttached::~UCListItemAttached()
{
}

void UCListItemAttached::connectToAttached(UCListItemAttached *parentAttached)
{
    setList(parentAttached->listItem(), UCListItemAttachedPrivate::get(parentAttached)->panel->isLeading());
}

void UCListItemAttached::setList(UCListItem *list, bool leading)
{
    Q_D(UCListItemAttached);
    if (d->m_listItem == list) {
        return;
    }
    d->m_listItem = list;
    Q_EMIT listItemChanged();

    d->panel = leading ? UCListItemPrivate::get(d->m_listItem)->leadingPanel : UCListItemPrivate::get(d->m_listItem)->trailingPanel;
    if (d->m_listItem) {
        // connect statusChanged() to update status, listItem, listItemIndex and overshoot values
        QObject::connect(d->panel, &UCActionPanel::statusChanged,
                         this, &UCListItemAttached::panelStatusChanged);

        // connect actions to get updates about visible changes
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(d->panel->actions())->actions) {
            QObject::connect(action, &UCAction::visibleChanged,
                             this, &UCListItemAttached::updateVisibleActions);
        }
        updateVisibleActions();
        Q_EMIT containerChanged();
        Q_EMIT visibleActionsChanged();
    }
}

// private slot to update visible actions
void UCListItemAttached::updateVisibleActions()
{
    Q_D(UCListItemAttached);
    d->m_visibleActions.clear();
    if (d->panel) {
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(d->panel->actions())->actions) {
            if (action->m_visible) {
                d->m_visibleActions << action;
            }
        }
    }
    Q_EMIT visibleActionsChanged();
}

/*!
 * \qmlattachedproperty ListItemActions ListItem::container
 * \readonly
 * The property holds the instance of the \l ListItemActions the ListItem's actions
 * panel is visualizing.
 */
UCListItemActions *UCListItemAttached::container() const
{
    Q_D(const UCListItemAttached);
    return d->panel ? d->panel->actions() : 0;
}

/*!
 * \qmlattachedproperty list<Action> ListItem::visibleActions
 * Holds the list of visible actions. This is a convenience property to help action
 * visualization panel implementations to consider only visible actions.
 */
QQmlListProperty<UCAction> UCListItemAttached::visibleActions()
{
    Q_D(UCListItemAttached);
    return QQmlListProperty<UCAction>(this, d->m_visibleActions);
}

/*!
 * \qmlattachedproperty ListItem ListItem::listItem
 * \readonly
 * The property reports the connected \l ListItem to the list of actions.
 */
UCListItem *UCListItemAttached::listItem()
{
    Q_D(UCListItemAttached);
    return d->m_listItem;
}

/*!
 * \qmlattachedproperty int ListItem::listItemIndex
 * \readonly
 * Holds the index of the \l ListItem within a view, if the \l ListItem is used
 * in a model driven view. Otherwise it is set to -1.
 */
int UCListItemAttached::listItemIndex() {
    Q_D(UCListItemAttached);
    return d->m_listItem ? UCListItemPrivate::get(d->m_listItem)->index() : -1;
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
UCListItem::PanelStatus UCListItemAttached::panelStatus()
{
    Q_D(UCListItemAttached);
    if (!d->panel) {
        return UCListItem::Disconnected;
    }
    return d->panel->panelStatus();
}

/*!
 * \qmlattachedmethod void ListItem::snapToPosition(real position)
 * The function can be used to perform custom snapping, or to execute rebounding
 * and also disconnecting from the connected \l ListItem. This can be achieved by
 * calling the function with 0.0 value.
 */
void UCListItemAttached::snapToPosition(qreal position)
{
    UCListItem::PanelStatus itemStatus = panelStatus();
    Q_D(UCListItemAttached);
    //if it is disconnected, leave (this also includes the case when m_container is null)
    if (!d->m_listItem || !d->panel || itemStatus == UCListItem::Disconnected) {
        return;
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(d->m_listItem);
    position *= (itemStatus == UCListItem::Leading) ? 1 : -1;
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
