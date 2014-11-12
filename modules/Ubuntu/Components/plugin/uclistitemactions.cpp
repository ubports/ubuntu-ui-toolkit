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
#include "plugin.h"
#include <QtQml/QQmlInfo>
#include "ucaction.h"
#include "ucunits.h"
#include "uclistitemstyle.h"

UCListItemActionsPrivate::UCListItemActionsPrivate()
    : QObjectPrivate()
    , status(UCListItemActions::Disconnected)
    , offsetDragged(0)
    , delegate(0)
    , panelDelegate(0)
    , panelItem(0)
{
}
UCListItemActionsPrivate::~UCListItemActionsPrivate()
{
}

void UCListItemActionsPrivate::_q_updateDraggedOffset()
{
    UCListItem *listItem = qobject_cast<UCListItem*>(panelItem->parentItem());
    if (!listItem) {
        return;
    }

    Q_Q(UCListItemActions);
    offsetDragged = (status == UCListItemActions::Leading) ? panelItem->width() + panelItem->x() :
                         listItem->width() - panelItem->x();
    if (offsetDragged < 0.0) {
        offsetDragged = 0.0;
    }
}

UCListItemActionsAttached *UCListItemActionsPrivate::attachedObject()
{
    if (!panelItem) {
        return 0;
    }
    return static_cast<UCListItemActionsAttached*>(
                qmlAttachedPropertiesObject<UCListItemActions>(panelItem, false));
}

/*
 * Connects a ListItem to the ListItemActions' panel and returns true upon successful connection.
 * Connection may fail if there is a different ListItem connected. In this case the new ListItem
 * will be queued and automatically connected when the previous ListItem disconnects.
 * The panel is only re-created when a different Component is used to create it.
 * FIXME - despite each ListItem uses the same document to create the panel, theming engine
 * provides different Component objects for each, due to not caching the components created.
 * This must be fixed to optimize memory usage.
 */
bool UCListItemActionsPrivate::connectToListItem(UCListItemActions *actions, UCListItem *listItem, bool leading)
{
    UCListItemActionsPrivate *_this = get(actions);
    if (!_this) {
        return false;
    }
    // do we have a panel created already?
    if (_this->panelItem) {
        if (isConnectedTo(actions, listItem)) {
            return true;
        }
        if (_this->panelItem->parentItem() && _this->panelItem->parentItem() != listItem) {
            // set the requesting listItem as queuedItem
            _this->queuedItem = listItem;
            return false;
        }
    }
    // no parent set or panelItem yet, proceed with panel creation
    UCListItemPrivate *pItem = UCListItemPrivate::get(listItem);
    if (!pItem->styleItem && pItem->loadStyle()) {
        pItem->initStyleItem();
    }
    if (!pItem->styleItem || (pItem->styleItem && !_this->createPanelItem(pItem->styleItem->m_actionsDelegate))) {
        return false;
    }

    _this->panelItem->setParentItem(listItem);
    if (_this->attachedObject()) {
        _this->attachedObject()->connectListItem(listItem, true);
    }
    _this->offsetDragged = 0.0;
    _this->status = leading ? UCListItemActions::Leading : UCListItemActions::Trailing;
    Q_EMIT actions->statusChanged(_this->status);
    return true;
}

void UCListItemActionsPrivate::disconnectFromListItem(UCListItemActions *actions)
{
    UCListItemActionsPrivate *_this = get(actions);
    if (!_this || !_this->panelItem || !_this->panelItem->parentItem()) {
        return;
    }

    if (_this->attachedObject()) {
        _this->attachedObject()->connectListItem(static_cast<UCListItem*>(_this->panelItem->parentItem()), false);
    }
    _this->panelItem->setParentItem(0);
    _this->status = UCListItemActions::Disconnected;
    Q_EMIT actions->statusChanged(_this->status);
    // if there was a queuedItem, make it grab the actions list
    if (_this->queuedItem) {
        UCListItemPrivate::get(_this->queuedItem.data())->grabPanel(actions, true);
        // remove item from queue
        _this->queuedItem.clear();
    }
}

bool UCListItemActionsPrivate::isConnectedTo(UCListItemActions *actions, UCListItem *listItem)
{
    UCListItemActionsPrivate *_this = get(actions);
    return _this && _this->panelItem &&
            (_this->status != UCListItemActions::Disconnected) &&
            (_this->panelItem->parentItem() == listItem);
}

QQuickItem *UCListItemActionsPrivate::createPanelItem(QQmlComponent *panel)
{
    if (panelItem && panelDelegate == panel) {
        return panelItem;
    }
    // delete the panel if the next item's actionsDelegate differs from the
    // one this panel was created from
    if (panelDelegate != panel) {
        delete panelItem;
        panelItem = 0;
    }

    Q_Q(UCListItemActions);
    if (!panel) {
        qmlInfo(q) << UbuntuI18n::instance().tr("actionsDelegate not set!");
        return 0;
    }

    panelDelegate = panel;
    if (!panelDelegate->isError()) {
        panelItem = qobject_cast<QQuickItem*>(panelDelegate->beginCreate(qmlContext(q)));
        if (panelItem) {
            QQml_setParent_noEvent(panelItem, q);
            // add panelItem to data so we can access it in case is needed (i.e. tests)
            data.append(panelItem);
            // create attached property!
            UCListItemActionsAttached *attached = static_cast<UCListItemActionsAttached*>(
                        qmlAttachedPropertiesObject<UCListItemActions>(panelItem));
            if (attached) {
                if (!attached->container()) {
                    attached->setList(q);
                } else {
                    // container is set, but we need to emit the signal again so we get the
                    // attached props updated for those cases when the attached property is
                    // created before the statement above
                    Q_EMIT attached->containerChanged();
                }
            }
            panelDelegate->completeCreate();

            // calculate option's slot size
            offsetDragged = 0.0;
            QObject::connect(panelItem, SIGNAL(xChanged()), q, SLOT(_q_updateDraggedOffset()));
        }
    } else {
        qmlInfo(q) << panelDelegate->errorString();
    }

    return panelItem;
}

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
 * on the \l {ListItemStyle::snapAnimation}{snapAnimation} duration, this may take some time, and the
 * response time of the UI can become unacceptable.
 *
 * Having individual ListItemActions instances increases the memory footprint,
 * however the UI will be more responsive as swiping individual ListItems will
 * not have to wait till the previous ListItem's panel is snapped out (rebound).
 * On the other hand, memory consumption will increase significantly due to
 * separate panel creation, and performance may decrease with up to 40%, depending
 * on what way are the actions declared, within the ListItemActions or as shared
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
 * \section3 Action parameter types
 * Actions handled by the ListItem are all triggered with the ListItem's index
 * as parameter. This index can either be the model index when used with ListView,
 * or the child index from the parentItem's childItems list. Actions can use this
 * parameter to identify the instance of the ListItem on which it was executed.
 * However this will only work if the \l {Action::parameterType}{parameterType}
 * will be set to Action.Integer or not set at all, in which case the ListItem
 * will set the type to Action.Integer.
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
 * The property holds the custom delegate to visualize the actions listed in the
 * ListItemActions. When set to null, the default delegate specified by the \l
 * ListItem::actionsDelegate will be used.
 *
 * ListItemActions provides the \c action context property which contains the
 * Action instance currently visualized. Using this property delegates can access
 * the information to be visualized. The action is triggered by the panel item
 * holding the visualized action, therefore only visualization is needed by the
 * custom delegate. The other context property exposed to delegates is the \c
 * index, which specifies the index of the action visualized.
 *
 * The delegate height is set automatically by the panel item, and the width value
 * is clamped between height and the maximum width of the list item divided by the
 * number of actions in the list.
 * \qml
 * import QtQuick 2.2
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
 *                     color: "blue"
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
 * of the panel.
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

#include "moc_uclistitemactions.cpp"
