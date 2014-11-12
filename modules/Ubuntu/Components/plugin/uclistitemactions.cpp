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

#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "uclistitem_p.h"
#include "quickutils.h"
#include "i18n.h"
#include <QtQml/QQmlInfo>

UCListItemActionsPrivate::UCListItemActionsPrivate()
    : QObjectPrivate()
    , status(UCListItemActions::Disconnected)
    , offsetDragged(0)
    , delegate(0)
    , panelItem(0)
{
}
UCListItemActionsPrivate::~UCListItemActionsPrivate()
{
}

// FIXME - fis ListItemStyle::snapAnimation to a link when property available
/*!
 * \qmltype ListItemActions
 * \instantiates UCListItemActions
 * \inherits QtQObject
 * \inqmlmodule Ubuntu.Components 1.2
 * \since Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \brief Provides configuration for actions to be added to a ListItem.
 *
 * ListItem accepts actions that can be configured to appear when swiped to left
 * or right. The API does not limit the number of actions to be assigned for leading
 * or trailing actions, however the design constrains are allowing a maximum of
 * 1 action on leading- and a maximum of 3 actions on trailing side of the ListItem.
 *
 * The \l actions are Action instances or elements derived from Action. The default
 * visualization of the actions can be overridden using the \l delegate property,
 * and the default implementation uses the \c name property of the Action.
 *
 * The leading and trailing actions are placed on a panel item, which is created
 * the first time the actions are accessed. The colors of the panel is taken from
 * the theme's palette.
 *
 * When swiped, panels reveal the actions one by one. In case an action is revealed
 * more than 50%, the action will be snapped and revealed completely. This is also
 * valid for the case when the action is visible less than 50%, in which case the
 * action is hidden. Actions can be triggered by tapping.
 *
 * \note You can use the same ListItemActions for leading and for trailing actions
 * the same time only if the instance is used by different groups of list items,
 * where one group uses it as leading and other group as trailing. In any other
 * circumstances use separate ListItemActions for leading and trailing actions.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.2
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     ListItemActions {
 *         id: sharedActions
 *         actions: [
 *             Action {
 *                 iconName: "search"
 *             },
 *             Action {
 *                 iconName: "edit"
 *             },
 *             Action {
 *                 iconName: "copy"
 *             }
 *         ]
 *     }
 *
 *     Column {
 *         ListItem {
 *             leadingActions: sharedActions
 *         }
 *         UbuntuListView {
 *             anchors.fill: parent
 *             model: 10000
 *             delegate: ListItem {
 *                 trailingActions: sharedActions
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * \section3 Using with ListViews
 * ListItemActions instances can be shared between ListItem instances within the
 * same view. When shared, the memory footprint of the view will be lot smaller,
 * as there will be no individual panel created for each list's actions visualization.
 * Depending on how long the initialization of the component used in \c ListItem::actionsDelegate
 * takes, creation time will be also reduced to one time per view.
 * However, this implies that swiping a new ListItem content while another one is
 * swiped will result in showing the newly swiped item's panel delayed, as the
 * panel can be shown only after the previous item's snapping is completed. Depending
 * on the \c ListItemStyle::snapAnimation duration, this may take some time, and the
 * response time of the UI can become unacceptable.
 *
 * Having individual ListItemActions instances increases the memory footprint,
 * however the UI will be more responsive as swiping individual ListItems will
 * not have to wait till the previous ListItem's panel is snapped out (rebount).
 * On the other hand, memory consumption will increase signifficantly doe to
 * separate panel creation, and performance may decrease with up to 40%, depending
 * on how "badly" are the actions declared, within the ListItemActions or as shared
 * actions.
 *
 * The example above illustrates how to share ListItemActions between ListItem
 * delegates, which can be the worst-performant but most lightwaight memory consumer
 * setup. The following example illustrates the worst case:
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.2
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     UbuntuListView {
 *         anchors.fill: parent
 *         model: 10000
 *         delegate: ListItem {
 *             leadingActions: ListItemActions {
 *                 actions: [
 *                     Action {
 *                         iconName: "delete"
 *                     }
 *                 ]
 *             }
 *             trailingActions: ListItemActions {
 *                 actions: [
 *                     Action {
 *                         iconName: "search"
 *                     },
 *                     Action {
 *                         iconName: "edit"
 *                     },
 *                     Action {
 *                         iconName: "copy"
 *                     }
 *                 ]
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * This example can be optimized by sharing the action arrays between the items:
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.2
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     property list<Action> leading: [
 *         Action {
 *             iconName: "delete"
 *         }
 *     ]
 *     property list<Action> trailing: [
 *         Action {
 *             iconName: "search"
 *         },
 *         Action {
 *             iconName: "edit"
 *         },
 *         Action {
 *             iconName: "copy"
 *         }
 *     ]
 *
 *     UbuntuListView {
 *         anchors.fill: parent
 *         model: 10000
 *         delegate: ListItem {
 *             leadingActions: ListItemActions {
 *                 actions: leading
 *             }
 *             trailingActions: ListItemActions {
 *                 actions: trailing
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * \section3 Attached properties
 * ListItemActions provides a set of attached properties to the panels visualizing
 * the actions. These properties can be used by implementations visualizing the
 * actions.
 */

UCListItemActions::UCListItemActions(QObject *parent)
    : QObject(*(new UCListItemActionsPrivate), parent)
{
}
UCListItemActions::~UCListItemActions()
{
}

UCListItemActionsAttached *UCListItemActions::qmlAttachedProperties(QObject *owner)
{
    /*
     * Detect the attachee, whether is it a child item of the panelItem. The panelItem
     * itself cannot be detected, as the object can be attached during the call of
     * component.beginCreate().
     */
    UCListItemActionsAttached *attached = new UCListItemActionsAttached(owner);
    QQuickItem *item = qobject_cast<QQuickItem*>(owner);
    while (item) {
        // has item our attached property?
        UCListItemActionsAttached *itemAttached = static_cast<UCListItemActionsAttached*>(
                    qmlAttachedPropertiesObject<UCListItemActions>(item, false));
        if (itemAttached) {
            attached->setList(itemAttached->container());
            break;
        }
        item = item->parentItem();
    }
    return attached;
}

/*!
 * \qmlproperty Component ListItemActions::delegate
 * Custom delegate which overrides the default one used by the ListItem. If the
 * value is null, the default delegate will be used.
 *
 * Defaults to null.
 */
QQmlComponent *UCListItemActions::delegate() const
{
    Q_D(const UCListItemActions);
    return d->delegate;
}
void UCListItemActions::setDelegate(QQmlComponent *delegate)
{
    Q_D(UCListItemActions);
    if (d->delegate == delegate) {
        return;
    }
    d->delegate = delegate;
    Q_EMIT delegateChanged();
}

/*!
 * \qmlproperty list<Action> ListItemActions::actions
 * \default
 * The property holds the actions to be displayed. It can hold instances cached or
 * declared in place. An example of cached actions:
 * \qml
 * ListItemActions {
 *     id: cachedActions
 *     actions: [
 *         copyAction, searchAction, cutAction
 *     ]
 * }
 * \endqml
 */
QQmlListProperty<UCAction> UCListItemActions::actions()
{
    Q_D(UCListItemActions);
    return QQmlListProperty<UCAction>(this, d->actions);
}

/*!
 * \internal
 * \qmlproperty list<QtObject> ListItemActions::data
 * \default
 * The property holds any additional content added to the ListItemActions.
 */
QQmlListProperty<QObject> UCListItemActions::data()
{
    Q_D(UCListItemActions);
    return QQmlListProperty<QObject>(this, d->data);
}
