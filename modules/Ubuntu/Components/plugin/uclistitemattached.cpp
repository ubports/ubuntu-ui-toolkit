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
#include <QtQuick/private/qquickflickable_p.h>

/*!
 * \qmlattachedsignal ListItem::rebound()
 * The signal is emitted when the ListItem detects a mouse press or tap occurred
 * outside of the ListItem while its content is swiped (leading or trailing action
 * panel is shown). Style implementations can handle this signal to snap out the
 * panel.
 */

UCListItemAttached::UCListItemAttached(QObject *parent)
    : QObject(*(new UCListItemAttachedPrivate()), parent)
{
}

UCListItemAttached::~UCListItemAttached()
{
}

void UCListItemAttached::setList(UCListItem *list, bool leading, bool visualizeActions)
{
    Q_D(UCListItemAttached);
    if (d->listItem == list) {
        return;
    }
    d->listItem = list;

    if (visualizeActions) {
        d->panel = leading ? UCListItemPrivate::get(d->listItem)->leadingPanel : UCListItemPrivate::get(d->listItem)->trailingPanel;
    } else {
        d->panel = 0;
    }
    if (d->panel) {
        // connect statusChanged() to update status, listItem, listItemIndex and overshoot values
        QObject::connect(d->panel, &UCActionPanel::statusChanged,
                         this, &UCListItemAttached::panelStatusChanged);

        // connect actions to get updates about visible changes
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(d->panel->actions())->actions) {
            QObject::connect(action, &UCAction::visibleChanged,
                             this, &UCListItemAttached::updateVisibleActions);
        }
        updateVisibleActions();
        Q_EMIT flickableChanged();
        Q_EMIT actionsChanged();
        Q_EMIT visibleActionsChanged();
    }
}

// private slot to update visible actions
void UCListItemAttached::updateVisibleActions()
{
    Q_D(UCListItemAttached);
    if (d->panel) {
        d->visibleActions.clear();
        Q_FOREACH(UCAction *action, UCListItemActionsPrivate::get(d->panel->actions())->actions) {
            if (action->m_visible) {
                d->visibleActions << action;
            }
        }
        Q_EMIT visibleActionsChanged();
    }
}

/*!
 * \qmlattachedproperty ListItemActions ListItem::actions
 * \readonly
 * The property holds the instance of the \l ListItemActions the ListItem's actions
 * panel is visualizing. The property holds a valid value only when attached to
 * the action visualizing panel defined in \l ListItemStyle::actionsDelegate style
 * property.
 */
UCListItemActions *UCListItemAttached::actions() const
{
    Q_D(const UCListItemAttached);
    return d->panel ? d->panel->actions() : 0;
}

/*!
 * \qmlattachedproperty list<Action> ListItem::visibleActions
 * Holds the list of visible actions. This is a convenience property to help action
 * visualization panel implementations to consider only visible actions. Similar to
 * \l actions attached property, it is only valid when attached to \l ListItemStyle::actionsDelegate
 * component.
 */
QQmlListProperty<UCAction> UCListItemAttached::visibleActions()
{
    Q_D(UCListItemAttached);
    return QQmlListProperty<UCAction>(this, d->visibleActions);
}

/*!
 * \qmlattachedproperty ListItem ListItem::item
 * \readonly
 * The property reports the connected \l ListItem to the panel. Valid in every attachee.
 */
UCListItem *UCListItemAttached::item()
{
    Q_D(UCListItemAttached);
    return d->listItem;
}

/*!
 * \qmlattachedproperty int ListItem::index
 * \readonly
 * Holds the index of the \l ListItem within a view, if the \l ListItem is used
 * in a model driven view, or the child index otherwise. Valid in every attachee.
 */
int UCListItemAttached::index() {
    Q_D(UCListItemAttached);
    return d->listItem ? UCListItemPrivate::get(d->listItem)->index() : -1;
}

/*!
 * \qmlattachedproperty enum ListItem::panelStatus
 * \readonly
 * The property holds the status of the ListItemActions, whether is connected
 * as leading or as trailing action list to a \l ListItem. Possible valueas are:
 * \list A
 *  \li \b None - default, the actions list is not connected to any \l ListItem
 *  \li \b Leading - the actions list is connected as leading list
 *  \li \b Trailing - the actions list is connected as trailing list
 * \endlist
 * The property is valid only when attached to \l ListItemStyle::actionsDelegate component.
 */
UCListItem::PanelStatus UCListItemAttached::panelStatus()
{
    Q_D(UCListItemAttached);
    if (!d->panel) {
        return UCListItem::None;
    }
    return d->panel->panelStatus();
}

/*!
 * \qmlattachedproperty Flickable ListItem::flickable
 * The property holds the Flickable owning the ListItem. The property contains a
 * valid Flickable if the ListItem is the delegate of a ListView or if its parent
 * Positioner (Column, Row, etc) is declared as content for a Flickable:
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 *
 * Flickable {
 *     width: units.gu(20)
 *     height: units.gu(50)
 *     contentHeight: column.height
 *
 *     Column {
 *         width: parent.width
 *         Repeater {
 *             model: 25
 *             ListItem {
 *                 // [...]
 *             }
 *         }
 *     }
 * }
 * \endqml
 */
QQuickFlickable *UCListItemAttached::flickable() const
{
    Q_D(const UCListItemAttached);
    return d->listItem ? UCListItemPrivate::get(d->listItem)->flickable : NULL;
}
