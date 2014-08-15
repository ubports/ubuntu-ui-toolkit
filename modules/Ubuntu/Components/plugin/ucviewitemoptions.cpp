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

#include "ucviewitemoptions.h"
#include "ucviewitemoptions_p.h"
#include "ucviewitem_p.h"

UCViewItemOptionsPrivate::UCViewItemOptionsPrivate(UCViewItemOptions *qq)
    : q_ptr(qq)
    , delegate(0)
    , panel(0)
    , backgroundColor(Qt::red)
{
}

bool UCViewItemOptionsPrivate::createPanel(bool isLeading)
{
    Q_Q(UCViewItemOptions);
    UCViewItem *viewItem = qobject_cast<UCViewItem*>(q->parent());
    if (!panel) {
        QQmlComponent component()
    }
}

/*!
 * \qmltype ViewItemOptions
 * \instantiates UCViewItemOptions
 * \inherits QtQObject
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief Provides configuration for options to be added to a ViewItem.
 *
 * ViewItem accepts options that can be configured to appear when swiped to left
 * or right. There is no limitation on how many options can be displayed on both
 * sides, this depends on the space available. However design guides say that it
 * can be a maximum of one option on the left (leadng) and a maximum of 3 options
 * on the right (trailing) side of a ViewItem.
 *
 * The options are Action instances or elements derived from Action. Visualization
 * of the options can be configured using the \l delegate property, and the default
 * implementation uses the \c name property of the Action.
 *
 * The leading and trailing options are placed on a panel which has different colors
 * for each side. This can be configured using the \l backgroundColor property.
 *
 * When tugged, panels reveal the options one by one. In case an option is revealed
 * more than 50%, the option will be snapped and revealed completely. Options can be
 * triggered by tapping.
 *
 * \section3 Notes on performance
 * When used with views, or when the amount of items of same kind to be created
 * is huge, it is recommended to use cached actions as well as cached ViewItemOption
 * instances. In this way we can reduce the creation time of the items:
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * MainView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *
 *     ViewItemModel {
 *         id: commonOptions
 *         Action {
 *             iconName: "search"
 *         }
 *         Action {
 *             iconName: "edit"
 *         }
 *         Action {
 *             iconName: "copy"
 *         }
 *     }
 *     ListView {
 *         anchors.fill: parent
 *         model: 10000
 *         delegate: ViewItem {
 *             trailingOptions: commonOptions
 *         }
 *     }
 * }
 * \endqml
 */

UCViewItemOptions::UCViewItemOptions(QObject *parent)
    : QObject(parent)
    , d_ptr(new UCViewItemOptionsPrivate(this))
{
}
UCViewItemOptions::~UCViewItemOptions()
{
}

/*!
 * \qmlproperty Component ViewItemOptions::delegate
 * Custom delegate which overrides the default one used by the ViewItem. The default
 * value is null.
 */
PROPERTY_SETTER_PRIVATE_PTYPE(UCViewItemOptions, QQmlComponent, delegate)
PROPERTY_GETTER_PRIVATE(UCViewItemOptions, QQmlComponent*, delegate)
PROPERTY_RESET(UCViewItemOptions, delegate){}

/*!
 * \qmlproperty list<Action> ViewItemOptions::options
 * \default
 * The property holds the options to be displayed. It can hold instances cached or
 * declared in place. An example of cached options:
 * \qml
 * ViewItemOptions {
 *     id: cacedOptions
 *     options: [
 *         Action {
 *             iconName: "edit"
 *         },
 *         copyAction, searchAction, cutAction
 *     ]
 * }
 * \endqml
 */
LISTPROPERTY_GETTER_PRIVATE(UCViewItemOptions, QObject, options)

/*!
 * \qmlproperty color ViewItemOptions::backgroundColor
 * The color used to override the panel background color holding the visualized
 * options.
 */
PROPERTY_GETTER_PRIVATE(UCViewItemOptions, QColor, backgroundColor)
PROPERTY_SETTER_PRIVATE(UCViewItemOptions, QColor, backgroundColor)
PROPERTY_RESET(UCViewItemOptions, backgroundColor){}
