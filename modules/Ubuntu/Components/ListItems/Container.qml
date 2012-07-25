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
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief The visual parent of all list items

    Examples:
    \qml
        import Ubuntu.Components.ListItems as ListItem
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
Item {
    id: listItemContainer
    width: 250
    height: listItems.height

    /*!
      \internal
      Keep the ListItems that the user defines separate
      from the other items that we create below, and
      ensure the listItems column is their parent.
    */
    default property alias children: listItems.children
    Column {
        id: listItems
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        function updateSeparators() {
            var num = listItems.children.length;
            if (num > 0) {
                var item;
                for (var i = 0; i < num-1; i++) {
                    item = listItems.children[i];
                    if (item.__isDivider || listItems.children[i+1].__isDivider) {
                        item.__showBottomSeparator = false;
                    } else {
                        item.__showBottomSeparator = true;
                    }
                    item.__showTopSeparator = false;
                }
                if (!listItems.children[0].__isDivider) listItems.children[0].__showTopSeparator = true;
            }
        }

        onChildrenChanged: listItems.updateSeparators();
    }
}
