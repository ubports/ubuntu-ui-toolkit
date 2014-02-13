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

import QtQuick 2.0

/*!
    \qmltype UbuntuListView
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief A ListView with special features tailored for a look and feel fitting the
    Ubuntu Touch platform.
    The UbuntuListView works just like a regular ListView, but it adds special features
    such as expanding/collapsing items (when used together with the Expandable item).
    It provides features like automatically positioning the expanding item when it
    expands and collapsing it again when the user taps outside of it.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        Item {
            ListModel {
                id: listModel
            }

            UbuntuListView {
                anchors { left: parent.left; right: parent.right }
                height: units.gu(24)
                model: listModel

                delegate: ListItem.Expandable {
                    id: expandingItem

                    expandedHeight: units.gu(30)

                    onClicked: {
                        expanded = true;
                    }
                }
            }
        }
    \endqml

    \b{This component is under heavy development.}
*/

ListView {
    id: root

    /*!
      \preliminary
      Points to the currently expanded item. Null if no item is expanded.
      \qmlproperty Item expandedItem
     */
    readonly property alias expandedItem: priv.expandedItem

    /*!
      \preliminary
      Expand the given item. The item must be a child of this ListView.
     */
    function expandItem(item) {
        if (!item.hasOwnProperty("expandedHeight") || !item.hasOwnProperty("collapsedHeight")) {
            return;
        }

        if (priv.expandedItem != null) {
            priv.expandedItem.expanded = false;
        }
        item.expanded = true;
        priv.expandedItem = item;

        var itemY = contentItem.mapFromItem(item).y;
        var itemHeight = item.expandedHeight;
        var itemIndex = indexAt(item.x, item.y);

        var bottomSpacing = itemIndex == model.count - 1 ? 0 : item.collapsedHeight;
        if (itemY + itemHeight > height + contentY - bottomSpacing) {
            contentY = Math.min(itemY, itemY + itemHeight - height + bottomSpacing);
        }
    }

    /*!
      \preliminary
      Collapse the currently expanded item. If there isn't any item expanded, this function does nothing.
     */
    function collapse() {
        priv.expandedItem.expanded = false;
        priv.expandedItem = null;
    }

    /*! \internal */
    QtObject {
        id: priv

        /*! \internal
          Points to the currently expanded item. Null if no item is expanded.
         */
        property Item expandedItem: null
    }

    Behavior on contentY {
        UbuntuNumberAnimation {}
    }

    MouseArea {
        parent: contentItem
        anchors.fill: parent
        z: 2
        enabled: priv.expandedItem != null
        onClicked: root.collapse();
    }
}
