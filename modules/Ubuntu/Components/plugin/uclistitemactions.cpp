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

UCListItemActionsPrivate::UCListItemActionsPrivate()
    : QObjectPrivate()
    , actionsFailure(false)
    , connected(false)
    , leading(false)
    , delegate(0)
    , panelItem(0)
    , optionSlotWidth(0.0)
    , offsetDragged(0.0)
    , optionsVisible(0)
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
    offsetDragged = (leading) ? panelItem->width() + panelItem->x() :
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

/*
 * Callback functions for QQmlListProperty handling append(), count(), at() and clear() list functions.
 */
void UCListItemActionsPrivate::funcAppend(QQmlListProperty<QObject> *list, QObject *action)
{
    UCListItemActions *_this = static_cast<UCListItemActions*>(list->object);
    UCListItemActionsPrivate *plist = UCListItemActionsPrivate::get(_this);
    if (!QuickUtils::inherits(action, "Action")) {
        qmlInfo(_this) << UbuntuI18n::instance().tr(QString("Action at index %1 is not an Action or a derivate of it.").arg(plist->actions.size()));
        plist->actionsFailure = true;
        plist->actions.clear();
        return;
    }
    if (!plist->actionsFailure) {
        plist->actions.append(action);
    }
}
int UCListItemActionsPrivate::funcCount(QQmlListProperty<QObject> *list)
{
    UCListItemActions *_this = static_cast<UCListItemActions*>(list->object);
    UCListItemActionsPrivate *plist = UCListItemActionsPrivate::get(_this);
    return plist->actions.size();
}
QObject *UCListItemActionsPrivate::funcAt(QQmlListProperty<QObject> *list, int index)
{
    UCListItemActions *_this = static_cast<UCListItemActions*>(list->object);
    UCListItemActionsPrivate *plist = UCListItemActionsPrivate::get(_this);
    return plist->actions.at(index);
}
void UCListItemActionsPrivate::funcClear(QQmlListProperty<QObject> *list)
{
    UCListItemActions *_this = static_cast<UCListItemActions*>(list->object);
    UCListItemActionsPrivate *plist = UCListItemActionsPrivate::get(_this);
    plist->actionsFailure = false;
    return plist->actions.clear();
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
    _this->leading = leading;
    _this->panelItem->setProperty("listItemIndex", UCListItemPrivate::get(listItem)->index);
    _this->panelItem->setProperty("leadingPanel", leading);
    _this->panelItem->setParentItem(listItem);
    _this->offsetDragged = 0.0;
    QObject::connect(_this->panelItem, SIGNAL(selected()), _this->panelItem->parentItem(), SLOT(_q_rebound()));
    _this->connected = true;
    return true;
}

void UCListItemActionsPrivate::disconnectFromListItem(UCListItemActions *actions)
{
    UCListItemActionsPrivate *_this = get(actions);
    if (!_this || !_this->panelItem || !_this->panelItem->parentItem()) {
        return;
    }

    QObject::disconnect(_this->panelItem, SIGNAL(selected()), _this->panelItem->parentItem(), SLOT(_q_rebound()));
    _this->connected = false;
    _this->leading = false;
    _this->panelItem->setParentItem(0);
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
    return _this && _this->panelItem && _this->connected && (_this->panelItem->parentItem() == listItem);
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
    return visible * _this->optionSlotWidth * (_this->leading ? 1 : -1);
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
            if (delegate) {
                panelItem->setProperty("delegate", QVariant::fromValue(delegate));
            }
            panelItem->setProperty("actionList", QVariant::fromValue(actions));
            component.completeCreate();
            Q_EMIT q->panelItemChanged();

            // calculate option's slot size
            offsetDragged = 0.0;
            optionsVisible = 0;
            _q_handlePanelWidth();
            // connect to panel to catch dragging
            QObject::connect(panelItem, SIGNAL(widthChanged()), q, SLOT(_q_handlePanelWidth()));
            QObject::connect(panelItem, SIGNAL(xChanged()), q, SLOT(_q_handlePanelDrag()));
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
 * 1 action on leading- and a maximum of 3 actions on trailing side of teh ListItem.
 *
 * The \l actions are Action instances or elements derived from Action. The default
 * visualization of the actions can be overridden using the \l delegate property,
 * and the default implementation uses the \c name property of the Action.
 *
 * The leading and trailing actions are placed on \l panelItem, which is created
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


/*!
 * \qmlproperty Component ListItemActions::delegate
 * Custom delegate which overrides the default one used by the ListItem. If the
 * value is null, the default delegate will be used.
 *
 * ListItemActions provides the \c action context property which contains the
 * Action instance currently visualized. Using this property delegates can access
 * the information to be visualized. The trigger is handled by the \l panelItem
 * therefore only visualization is needed by the custom delegates. The other
 * context property exposed to delegates is the \c index, which specifies the
 * index of the action visualized.
 *
 * The delegate height is set automatically by the panelItem, and the width value
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
QQmlListProperty<QObject> UCListItemActions::actions()
{
    Q_D(UCListItemActions);
    return QQmlListProperty<QObject>(this, &(d->actions),
                                     &UCListItemActionsPrivate::funcAppend,
                                     &UCListItemActionsPrivate::funcCount,
                                     &UCListItemActionsPrivate::funcAt,
                                     &UCListItemActionsPrivate::funcClear);
}

/*!
 * \qmlproperty Item ListItemActions::panelItem
 * The property presents the Item holding the visualized actions. The panel is
 * created when used the first time is used.
 */
QQuickItem *UCListItemActions::panelItem() const
{
    Q_D(const UCListItemActions);
    return d->panelItem;
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
