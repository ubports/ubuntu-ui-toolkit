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

#include "uclistitemoptions.h"
#include "uclistitemoptions_p.h"
#include "uclistitem_p.h"
#include "quickutils.h"
#include "i18n.h"
#include "plugin.h"
#include <QtQml/QQmlInfo>

UCListItemOptionsPrivate::UCListItemOptionsPrivate()
    : QObjectPrivate()
    , optionsFailure(false)
    , connected(false)
    , leading(false)
    , delegate(0)
    , panelItem(0)
{
}
UCListItemOptionsPrivate::~UCListItemOptionsPrivate()
{
}

void UCListItemOptionsPrivate::funcAppend(QQmlListProperty<QObject> *list, QObject *option)
{
    UCListItemOptions *_this = static_cast<UCListItemOptions*>(list->object);
    UCListItemOptionsPrivate *plist = UCListItemOptionsPrivate::get(_this);
    if (!QuickUtils::inherits(option, "Action")) {
        qmlInfo(_this) << UbuntuI18n::instance().tr(QString("Option at index %1 is not an Action or a derivate of it.").arg(plist->options.size()));
        plist->optionsFailure = true;
        plist->options.clear();
        return;
    }
    if (!plist->optionsFailure) {
        plist->options.append(option);
    }
}
int UCListItemOptionsPrivate::funcCount(QQmlListProperty<QObject> *list)
{
    UCListItemOptions *_this = static_cast<UCListItemOptions*>(list->object);
    UCListItemOptionsPrivate *plist = UCListItemOptionsPrivate::get(_this);
    return plist->options.size();
}
QObject *UCListItemOptionsPrivate::funcAt(QQmlListProperty<QObject> *list, int index)
{
    UCListItemOptions *_this = static_cast<UCListItemOptions*>(list->object);
    UCListItemOptionsPrivate *plist = UCListItemOptionsPrivate::get(_this);
    return plist->options.at(index);
}
void UCListItemOptionsPrivate::funcClear(QQmlListProperty<QObject> *list)
{
    UCListItemOptions *_this = static_cast<UCListItemOptions*>(list->object);
    UCListItemOptionsPrivate *plist = UCListItemOptionsPrivate::get(_this);
    plist->optionsFailure = false;
    return plist->options.clear();
}

bool UCListItemOptionsPrivate::connectToListItem(UCListItemOptions *options, UCListItem *listItem, bool leading)
{
    UCListItemOptionsPrivate *_this = get(options);
    if (!_this || !_this->createPanelItem() || isConnectedTo(options, listItem)) {
        return isConnectedTo(options, listItem);
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
    _this->panelItem->setProperty("leadingPanel", leading);
    _this->panelItem->setParentItem(listItem);
    _this->connected = true;
    return true;
}

void UCListItemOptionsPrivate::disconnectFromListItem(UCListItemOptions *options)
{
    UCListItemOptionsPrivate *_this = get(options);
    if (!_this || !_this->panelItem || !_this->panelItem->parentItem()) {
        return;
    }
    _this->panelItem->setParentItem(0);
    _this->connected = false;
    _this->leading = false;
    // if there was a queuedItem, make it grab the options list
    if (_this->queuedItem) {
        UCListItemPrivate::get(_this->queuedItem.data())->grabPanel(options, true);
        // remove item from queue
        _this->queuedItem.clear();
    }
}

bool UCListItemOptionsPrivate::isConnectedTo(UCListItemOptions *options, UCListItem *listItem)
{
    UCListItemOptionsPrivate *_this = get(options);
    return _this && _this->panelItem && _this->connected && (_this->panelItem->parentItem() == listItem);
}

QQuickItem *UCListItemOptionsPrivate::createPanelItem()
{
    if (panelItem) {
        return panelItem;
    }
    Q_Q(UCListItemOptions);
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
            panelItem->setProperty("optionList", QVariant::fromValue(options));
            component.completeCreate();
            Q_EMIT q->panelItemChanged();
        }
    } else {
        qmlInfo(q) << component.errorString();
    }
    return panelItem;
}

/*!
 * \qmltype ListItemOptions
 * \instantiates UCListItemOptions
 * \inherits QtQObject
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief Provides configuration for options to be added to a ListItem.
 *
 * ListItem accepts options that can be configured to appear when tugged to left
 * or right. There is no limitation on how many options can be displayed on both
 * sides, this depends on the space available. However design guides say that it
 * can be a maximum of one option on the left (leadng) and a maximum of 3 options
 * on the right (trailing) side of a ListItem.
 *
 * The options are Action instances or elements derived from Action. The default
 * visualization of the options can be overridden using the \l delegate property,
 * and the default implementation uses the \c name property of the Action.
 *
 * The leading and trailing options are placed on \l panelItem, which is created
 * the first time the options are accessed. The colors of the panel is taken from
 * the theme's palette.
 *
 * When tugged, panels reveal the options one by one. In case an option is revealed
 * more than 50%, the option will be snapped and revealed completely. This is also
 * valid for the case when the option is visibl eless than 50%, in which case the
 * option is hidden. Options can be triggered by tapping.
 *
 * \note You can use the same ListItemOptions for leading and for trailing options
 * the same time only if the options are used in a ListView or in a list where the
 * list items are scrolled by the same Flickable. In any other circumstances use
 * separate ListItemOptions for leading and trailing options.
 *
 * \section3 Notes on performance
 * When used with views, or when the amount of items of same kind to be created
 * is huge, it is recommended to use cached actions as well as cached ListItemOption
 * instances. In this way we can reduce the creation time of the items:
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     UbuntuListView {
 *         anchors.fill: parent
 *         model: 10000
 *         ListItemOptions {
 *             id: commonOptions
 *             Action {
 *                 iconName: "search"
 *             }
 *             Action {
 *                 iconName: "edit"
 *             }
 *             Action {
 *                 iconName: "copy"
 *             }
 *         }
 *         delegate: ListItem {
 *             trailingOptions: commonOptions
 *         }
 *     }
 * }
 * \endqml
 */

UCListItemOptions::UCListItemOptions(QObject *parent)
    : QObject(*(new UCListItemOptionsPrivate), parent)
{
}
UCListItemOptions::~UCListItemOptions()
{
}


/*!
 * \qmlproperty Component ListItemOptions::delegate
 * Custom delegate which overrides the default one used by the ListItem. If the
 * value is null, the default delegate will be used.
 *
 * Defaults to null.
 */
QQmlComponent *UCListItemOptions::delegate() const
{
    Q_D(const UCListItemOptions);
    return d->delegate;
}
void UCListItemOptions::setDelegate(QQmlComponent *delegate)
{
    Q_D(UCListItemOptions);
    if (d->delegate == delegate) {
        return;
    }
    d->delegate = delegate;
    Q_EMIT delegateChanged();
}

/*!
 * \qmlproperty list<Action> ListItemOptions::options
 * \default
 * The property holds the options to be displayed. It can hold instances cached or
 * declared in place. An example of cached options:
 * \qml
 * ListItemOptions {
 *     id: cacedOptions
 *     options: [
 *         copyAction, searchAction, cutAction
 *     ]
 * }
 * \endqml
 */
QQmlListProperty<QObject> UCListItemOptions::options()
{
    Q_D(UCListItemOptions);
    return QQmlListProperty<QObject>(this, &(d->options),
                                     &UCListItemOptionsPrivate::funcAppend,
                                     &UCListItemOptionsPrivate::funcCount,
                                     &UCListItemOptionsPrivate::funcAt,
                                     &UCListItemOptionsPrivate::funcClear);
}

/*!
 * \qmlproperty Item ListItemOptions::panelItem
 * The property presents the Item holding the visualized options. The panel is
 * created when used the first time is used.
 */
QQuickItem *UCListItemOptions::panelItem() const
{
    Q_D(const UCListItemOptions);
    return d->panelItem;
}

#include "moc_uclistitemoptions.cpp"
