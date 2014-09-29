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

UCListItemActionsPrivate::UCListItemActionsPrivate()
    : QObjectPrivate()
    , dragging(false)
    , delegate(0)
    , panelItem(0)
    , optionSlotWidth(0.0)
    , offsetDragged(0.0)
    , optionsVisible(0)
    , status(UCListItemActions::Disconnected)
{
}
UCListItemActionsPrivate::~UCListItemActionsPrivate()
{
}

void UCListItemActionsPrivate::_q_handlePanelDrag()
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
    if (optionSlotWidth > 0.0) {
        optionsVisible = (int)trunc(offsetDragged / optionSlotWidth);
    }
}

void UCListItemActionsPrivate::_q_handlePanelWidth()
{
    // check how many options are visible && enabled
    // FIXME: use Actions API when moved to C++
    int count = 0;
    for (int i = 0; i < actions.count(); i++) {
        if (actions[i]->property("visible").toBool() && actions[i]->property("enabled").toBool()) {
            count++;
        }
    }
    optionSlotWidth = panelItem->width() / count;
    _q_handlePanelDrag();
}

UCListItemActionsAttached *UCListItemActionsPrivate::attachedObject()
{
    if (!panelItem) {
        return 0;
    }
    return static_cast<UCListItemActionsAttached*>(
                qmlAttachedPropertiesObject<UCListItemActions>(panelItem, false));
}


bool UCListItemActionsPrivate::connectToListItem(UCListItemActions *actions, UCListItem *listItem, bool leading)
{
    UCListItemActionsPrivate *_this = get(actions);
    if (!_this || !_this->createPanelItem() || isConnectedTo(actions, listItem)) {
        return isConnectedTo(actions, listItem);
    }
    // check if the panel is still connected to a ListItem
    // this may happen if there is a swipe over an other item while the previous
    // one is rebounding
    if (_this->panelItem->parentItem()) {
        // set the requesting listItem as queuedItem
        _this->queuedItem = listItem;
        return false;
    }
    _this->panelItem->setParentItem(listItem);
    _this->offsetDragged = 0.0;
    _this->status = leading ? UCListItemActions::Leading : UCListItemActions::Trailing;
    Q_EMIT actions->__statusChanged();
    return true;
}

void UCListItemActionsPrivate::disconnectFromListItem(UCListItemActions *actions)
{
    UCListItemActionsPrivate *_this = get(actions);
    if (!_this || !_this->panelItem || !_this->panelItem->parentItem()) {
        return;
    }

    _this->panelItem->setParentItem(0);
    _this->status = UCListItemActions::Disconnected;
    Q_EMIT actions->__statusChanged();
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

qreal UCListItemActionsPrivate::snap(UCListItemActions *options)
{
    UCListItemActionsPrivate *_this = get(options);
    if (!_this || !_this->panelItem) {
        return 0.0;
    }
    qreal ratio = _this->offsetDragged / _this->optionSlotWidth;
    int visible = _this->optionsVisible;
    if (ratio > 0.0 && (ratio - trunc(ratio)) > 0.5) {
        visible++;
    }
    return visible * _this->optionSlotWidth * (_this->status == UCListItemActions::Leading ? 1 : -1);
}

void UCListItemActionsPrivate::setDragging(UCListItemActions *actions, UCListItem *listItem, bool dragging)
{
    UCListItemActionsPrivate *_this = get(actions);
    if (!_this || !_this->panelItem || !isConnectedTo(actions, listItem) || (_this->dragging == dragging)) {
        return;
    }

    _this->dragging = dragging;
    Q_EMIT actions->__draggingChanged();
}

QQuickItem *UCListItemActionsPrivate::createPanelItem()
{
    if (panelItem) {
        return panelItem;
    }
    Q_Q(UCListItemActions);
    QUrl panelDocument = UbuntuComponentsPlugin::pluginUrl().
            resolved(QUrl::fromLocalFile("ListItemPanel.qml"));
    QQmlComponent component(qmlEngine(q), panelDocument);
    if (!component.isError()) {
        panelItem = qobject_cast<QQuickItem*>(component.beginCreate(qmlContext(q)));
        if (panelItem) {
            QQml_setParent_noEvent(panelItem, q);
            // add panelItem to data so we can access it in case is needed (i.e. tests)
            data.append(panelItem);
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
            component.completeCreate();

            // calculate option's slot size
            offsetDragged = 0.0;
            optionsVisible = 0;
            _q_handlePanelWidth();
            // connect to panel to catch dragging
            QObject::connect(panelItem, SIGNAL(widthChanged()), q, SLOT(_q_handlePanelWidth()));
            QObject::connect(panelItem, SIGNAL(xChanged()), q, SLOT(_q_handlePanelDrag()));
            if (attached) {
                QObject::connect(panelItem, &QQuickItem::xChanged,
                                 attached, &UCListItemActionsAttached::offsetChanged);
            }
        }
    } else {
        qmlInfo(q) << component.errorString();
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
 * ListItem accepts actions that can be configured to appear when tugged to left
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
 * When tugged, panels reveal the actions one by one. In case an action is revealed
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
 * When used with views, or when the amount of items of same kind to be created
 * is huge, it is recommended to use cached ListItemActions instances to reduce
 * creation time and to be able to handle rebounding and flicking properly. If
 * each ListItem crteates its own ListItemActions instance the Flickable view may
 * be blocked and action visualization will also break.
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
 *         model: 10000
 *         ListItemActions {
 *             id: commonActions
 *             actions: [
 *                 Action {
 *                     iconName: "search"
 *                 },
 *                 Action {
 *                     iconName: "edit"
 *                 },
 *                 Action {
 *                     iconName: "copy"
 *                 }
 *             ]
 *         }
 *         delegate: ListItem {
 *             trailingActions: commonActions
 *         }
 *     }
 * }
 * \endqml
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
    UCListItemActionsAttached *attached = new UCListItemActionsAttached(owner);
    /*
     * Detect the attachee, whether is it the ListItemAction, or a child item of
     * the panelItem. The panelItem itself cannot be detected, as the object can be
     * attached during the call of component.beginCreate() from createItem().
     */
    // the only QObject we can attach this is ListItemActions itself, do not deal with any other QObject!
    UCListItemActions *actions = qobject_cast<UCListItemActions*>(owner);
    if (actions) {
        attached->setList(actions);
    } else {
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
    }
    return attached;
}

/*!
 * \qmlproperty Component ListItemActions::delegate
 * Custom delegate which overrides the default one used by the ListItem. If the
 * value is null, the default delegate will be used.
 *
 * ListItemActions provides the \c action context property which contains the
 * Action instance currently visualized. Using this property delegates can access
 * the information to be visualized. The action is triggered by the panel item
 * holding teh visualized action, therefore only visualization is needed by the
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
 *             trailingActions: optionsList
 *         }
 *         ListItemActions {
 *             id: optionsList
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
 *                     text: option.text + "#" + index
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
    if (d->panelItem) {
        // update panel's delegate as well
        d->panelItem->setProperty("delegate", QVariant::fromValue(delegate));
    }
    Q_EMIT delegateChanged();
}

/*!
 * \qmlproperty list<Action> ListItemActions::actions
 * The property holds the actions to be displayed. It can hold instances cached or
 * declared in place. An example of cached actions:
 * \qml
 * ListItemActions {
 *     id: cacedActions
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
