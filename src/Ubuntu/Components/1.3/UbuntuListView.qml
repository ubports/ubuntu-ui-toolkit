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

import QtQuick 2.4
import Ubuntu.Components 1.3 as Toolkit

/*!
    \qmltype UbuntuListView
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief A ListView with special features tailored for a look and feel fitting the
    Ubuntu Touch platform.
    The UbuntuListView works just like a regular ListView, but it adds special features
    such as expanding/collapsing items (when used together with the Expandable item).
    It provides features like automatically positioning the expanding item when it
    expands and collapsing it again when the user taps outside of it.

    Examples:
    \qml
        import Ubuntu.Components 1.3
        import Ubuntu.Components.ListItems 1.3 as ListItem

        Item {
            ListModel {
                id: listModel
            }

            UbuntuListView {
                id: ubuntuListView
                anchors { left: parent.left; right: parent.right }
                height: units.gu(24)
                model: listModel

                delegate: ListItem.Expandable {
                    id: expandingItem

                    expandedHeight: units.gu(30)

                    onClicked: {
                        ubuntuListView.expandedIndex = index;
                    }
                }
            }
        }
    \endqml
*/

ListView {
    id: root

    /*!
      The index of the currently expanded item. -1 if no item is expanded.
     */
    property int expandedIndex: -1

    QtObject {
        id: priv

        function positionViewAtIndexAnimated(expandedIndex) {
            animation.from = root.contentY;
            root.currentIndex = expandedIndex;
            if (expandedIndex == root.count - 1) {
                root.positionViewAtIndex(expandedIndex, ListView.End);
            } else {
                root.positionViewAtIndex(expandedIndex + 1, ListView.End);
            }

            var effectiveExpandedHeight = Math.min(root.currentItem.expandedHeight, root.height - root.currentItem.collapsedHeight);
            if (root.contentY - root.originY == 0) {
                if (((root.currentIndex + 1) * root.currentItem.collapsedHeight) + effectiveExpandedHeight > root.height) {
                    animation.to = ((root.currentIndex + 1) * root.currentItem.collapsedHeight + effectiveExpandedHeight) - root.height + root.originY
                } else {
                    animation.to = root.originY
                }
            } else {
                animation.to = root.contentY + (effectiveExpandedHeight - root.currentItem.collapsedHeight);
            }
            animation.start();
        }

        function requestFocus(reason) {
            // lookup for the currentItem, and if it is a FocusScope, focus the view
            // this will also focus the currentItem
            if (root.currentItem && root.currentItem.hasOwnProperty("activeFocusOnPress")) {
                root.forceActiveFocus(reason);
            }
        }
    }

    focus: true

    /*!
      \internal
      Grab focus when moved, flicked or clicked
     */
    onMovementStarted: priv.requestFocus(Qt.MouseFocusReason)
    onFlickStarted: priv.requestFocus(Qt.MouseFocusReason)
    Toolkit.Mouse.onClicked: priv.requestFocus(Qt.MouseFocusReason)

    /*!
      Expand the item at the given index.
     */
    onExpandedIndexChanged: {
        if (expandedIndex < 0) {
            return;
        }
        priv.positionViewAtIndexAnimated(expandedIndex, ListView.End)
    }

    UbuntuNumberAnimation {
        id: animation
        target: root
        property: "contentY"
    }

    MouseArea {
        parent: contentItem
        anchors.fill: parent
        z: 2
        enabled: root.expandedIndex != -1
        onClicked: root.expandedIndex = -1;
    }

    // animate move displaced
    moveDisplaced: Transition {
        UbuntuNumberAnimation {
            properties: "x,y"
        }
    }
}
