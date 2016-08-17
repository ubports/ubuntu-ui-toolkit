/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

/*!
    \qmltype ThinDivider
    \inqmlmodule Ubuntu.Components.ListItems 1.0
    \ingroup ubuntu-listitems
    \brief Narrow line used as a divider between ListItems.
    \note \b{The component is deprecated. Use ListItem component's divider property instead.}

    ListItems will usually include this line at the bottom of the item
    to give a visually pleasing list of items. But a divider line can
    also be inserted manually using this component.

    Examples:
    \qml
        import Ubuntu.Components.ListItems 1.3 as ListItem
        Column {
            width: 250
            ListItem.ThinDivider {} //can be used as header for list
            ListItem.Standard {
                text: "List Item without thin divider line"
                showDivider: false
            }
            ListItem.ThinDivider {} // manually inserted divider line
            ListItem.Standard {
                text: "ListItem with thin divider line"
            }
        }
    \endqml
*/
Rectangle {
    anchors {
        left: (parent) ? parent.left : null
        right: (parent) ? parent.right : null
    }
    height: (visible) ? units.dp(2) : 0
    // a private property to catch theme background color change
    // use private property instead of embedding it into a QtObject to avoid further
    // performance decrease
    property bool __lightBackground: ColorUtils.luminance(theme.palette.normal.background) > 0.85
    // use a gradient of 4 steps instead of instantiating two Rectangles for performance reasons
    gradient: Gradient {
        GradientStop { position: 0.0; color: __lightBackground ? Qt.rgba(0, 0, 0, 0.1) : Qt.rgba(0, 0, 0, 0.4) }
        GradientStop { position: 0.49; color: __lightBackground ? Qt.rgba(0, 0, 0, 0.1) : Qt.rgba(0, 0, 0, 0.4) }
        GradientStop { position: 0.5; color: __lightBackground ? Qt.rgba(1, 1, 1, 0.4) : Qt.rgba(1, 1, 1, 0.1) }
        GradientStop { position: 1.0; color: __lightBackground ? Qt.rgba(1, 1, 1, 0.4) : Qt.rgba(1, 1, 1, 0.1) }
    }
}
