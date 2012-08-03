/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 1.1

/*!
    \qmlclass Container
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief The visual parent of all list items

    Examples:
    \qml
        import Ubuntu.Components.ListItems 0.1 as ListItem
        ListItem.Container {
            ListItem.Standard {
                text: "Label"
                iconSource: "icon.png"
                selected: false
                onClicked: selected = !selected
            }
            ListItem.SingleValue {
                text: "Label"
                value: "Status"
                progression: true
            }
            ListItem.MultiValue {
                text: "Label"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            }
            ListItem.Standard {
                text: "Disabled"
                iconSource: "icon.png"
                enabled: false
            }
        }
    \endqml

    See \l Base and its subclasses for more examples.

    \b{This component is under heavy development.}
*/
Column {
    id: listItemContainer
    width: 250
}
