/*
 * Copyright 2014-2015 Canonical Ltd.
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
#include "ucaction.h"
#include "ucunits.h"
#include "uclistitemstyle.h"

UT_NAMESPACE_BEGIN

UCListItemActionsPrivate::UCListItemActionsPrivate()
    : QObjectPrivate()
    , delegate(0)
{
}
UCListItemActionsPrivate::~UCListItemActionsPrivate()
{
}

/*!
 * \qmltype ListItemActions
 * \instantiates UbuntuToolkit::UCListItemActions
 * \inherits QtQObject
 * \inqmlmodule Ubuntu.Components
 * \since Ubuntu.Components 1.2
 * \ingroup ubuntu-listitem
 * \brief Provides configuration for actions to be added to a ListItem.
 *
 * ListItem accepts actions that can be configured to appear when swiped to left
 * or right. The API does not limit the number of actions to be assigned for leading
 * or trailing actions, however the design constraints are allowing a maximum of
 * 1 action on leading- and a maximum of 3 actions on trailing side of the ListItem.
 *
 * The \l actions are Action instances or elements derived from Action. The default
 * visualization of the actions can be overridden using the \l delegate property,
 * and the default implementation uses the \c name property of the Action.
 *
 * \section3 Using with ListViews
 * ListItemActions instances can be shared between ListItem instances within the
 * same view. When shared, the memory footprint of the view will be lot smaller,
 * as there will be no individual action container created for each list's actions.
 * Having individual ListItemActions instances increases the memory footprint,
 * and also has performance impact on kinetic scrolling.
 *
 * The examples below illustrate the worst and best practice when used in a ListView.
 * The worst case:
 * \qml
 * import QtQuick 2.4
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
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     ListItemActions {
 *         id: leading
 *         actions: Action {
 *             iconName: "delete"
 *         }
 *     }
 *     ListItemActions {
 *         id: trailing
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
 *     UbuntuListView {
 *         anchors.fill: parent
 *         model: 10000
 *         delegate: ListItem {
 *             leadingActions: leading
 *             trailingActions: trailing
 *         }
 *     }
 * }
 * \endqml
 *
 * \section3 Action parameter types
 * Actions handled by the ListItem are all triggered with the ListItem's index
 * as parameter. This index can either be the model index when used with ListView,
 * or the child index from the parentItem's childItems list. Actions can use this
 * parameter to identify the instance of the ListItem on which it was executed,
 * in which case ListItem will change the type from \c Actions.None to \c Actions.Integer
 * when it is triggered.
 */

UCListItemActions::UCListItemActions(QObject *parent)
    : QObject(*(new UCListItemActionsPrivate), parent)
{
}
UCListItemActions::~UCListItemActions()
{
}

/*!
 * \qmlproperty Component ListItemActions::delegate
 * The property holds the custom delegate to visualize the actions listed in the
 * ListItemActions. When set to null, the default delegate specified by the \l
 * ListItemStyle will be used.
 *
 * ListItemActions provides the \c action context property which contains the
 * Action instance currently visualized. Using this property delegates can access
 * the information to be visualized. The action is triggered by the panel item
 * holding the visualized action, therefore only visualization is needed by the
 * custom delegate. The other context property exposed to delegates is the \c
 * index, which specifies the index of the action visualized.
 *
 * Specifying a custom delegate will not override the triggering logic of the
 * action, that will be still handled by the panel itself. However custom delegates
 * may still need to distinguish the pressed/released state visually. This can
 * be achieved using the \c pressed context property, which informs the delegate
 * about the pressed state of the action.
 *
 * The delegate height is set automatically by the panel item, only the width must
 * be specified which will be clamped between height and the maximum width of the
 * list item divided by the number of actions in the list.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     UbuntuListView {
 *         anchors.fill: parent
 *         model: 50
 *         delegate: ListItem {
 *             trailingActions: actionsList
 *         }
 *         ListItemActions {
 *             id: actionsList
 *             delegate: Column {
 *                 width: height + units.gu(2)
 *                 Icon {
 *                     name: action.iconName
 *                     width: units.gu(3)
 *                     height: width
 *                     color: pressed ? "blue" : "lightblue"
 *                     anchors.horizontalCenter: parent.horizontalCenter
 *                 }
 *                 Label {
 *                     text: action.text + "#" + index
 *                     width: parent.width
 *                     horizontalAlignment: Text.AlignHCenter
 *                 }
 *             }
 *             actions: Action {
 *                 iconName: "starred"
 *                 text: "Star"
 *             }
 *         }
 *     }
 * }
 * \endqml
 * \note Putting a Rectangle in the delegate can be used to override the color
 * of the panel. Also all ListItem attached properties can be used in the delegates.
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
int UCListItemActionsPrivate::actions_count(QQmlListProperty<UCAction> *p)
{
    return reinterpret_cast<QList<UCAction *> *>(p->data)->count();
}
void UCListItemActionsPrivate::actions_append(QQmlListProperty<UCAction> *p, UCAction *v)
{
    // check action type before adding it
    if (v->m_parameterType == UCAction::None) {
        v->setProperty("parameterType", UCAction::Integer);
    }
    reinterpret_cast<QList<UCAction *> *>(p->data)->append(v);
}
UCAction *UCListItemActionsPrivate::actions_at(QQmlListProperty<UCAction> *p, int i)
{
    return reinterpret_cast<QList<UCAction *> *>(p->data)->at(i);
}

void UCListItemActionsPrivate::actions_clear(QQmlListProperty<UCAction> *p)
{
    reinterpret_cast<QList<UCAction *> *>(p->data)->clear();
}

QQmlListProperty<UCAction> UCListItemActions::actions()
{
    Q_D(UCListItemActions);
    return QQmlListProperty<UCAction>(this, &d->actions,
                                      UCListItemActionsPrivate::actions_append,
                                      UCListItemActionsPrivate::actions_count,
                                      UCListItemActionsPrivate::actions_at,
                                      UCListItemActionsPrivate::actions_clear
                                      );
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

UT_NAMESPACE_END

#include "moc_uclistitemactions.cpp"
