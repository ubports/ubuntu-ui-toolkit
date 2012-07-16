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
import ".."

/*!
    \qmlclass Container
    \inqmlmodule UbuntuUIToolkit
    \brief The visual parent of all list items

    \qml
    Examples:
        Container {
            Default {
                text: "Label"
                iconSource: "icon.png"
                selected: false
                onClicked: selected = !selected
            }
            SingleValue {
                text: "Label"
                value: "Status"
                progression: true
            }
            MultiValue {
                text: "Label"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            }
            Default {
                text: "Disabled"
                iconSource: "icon.png"
                enabled: false
            }
        }
    \endqml

    See \l Default, \l SingleValue and \l MultiValue for more examples.

    \b{This component is under heavy development.}
*/
Item {
    id: listItemContainer
    width: 250

    /*!
      \internal
      Keep the ListItemsthat the user defines separate
      from the other items that we might create below.
    */
    default property alias children: listItems.children
    Column {
        id: listItems
        width: 250

        function updateChildren() {
            var num = listItems.children.length;
            if (num > 0) {
                var item;
                for (var i = 0; i < num; i++) {
                    item = listItems.children[i];
                    item.__isFirst = false;
                    item.__isLast = false;
                } // for
                listItems.children[0].__isFirst = true;
                listItems.children[num-1].__isLast = true;
            } // if
        } // function

        onChildrenChanged: updateChildren()
    }
}
