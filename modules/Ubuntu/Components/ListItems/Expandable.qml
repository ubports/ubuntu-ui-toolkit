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
import Ubuntu.Components 0.1

/*!
    \qmltype Expandable
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief An expandable list item with no contents.
    The Expandable class can be used for generic list items containing other
    components such as buttons. It subclasses \l Empty and thus brings all that
    functionality, but additionally provides means to expand and collapse the item.

    When used together with an UbuntuListView or \l ExpandablesColumn it
    can coordinate with other items in the list to make sure it is scrolled while
    expanding to be fully visible in the view. Additionally it is made sure that
    only one Expandable item is expanded at a time and it is collapsed when the
    user clicks outside the item.

    You can set \l expanded to true/false to expand/collapse the item.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        Item {
            ListModel {
                id: listModel
            }

            ListItem.UbuntuListView {
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

Empty {
    id: root
    implicitHeight: expanded ? priv.maxExpandedHeight : collapsedHeight

    /*!
      Reflects the expanded state. Set this to true/false to expand/collapse the item.
     */
    property bool expanded: false

    /*!
      The collapsed (normal) height of the item. Defaults to the standard height for list items.
     */
    property real collapsedHeight: __height

    /*!
      The expanded height of the item's content. Defaults to the same as collapsedHeight which
      disables the expanding feature. In order for the item to be expandable, set this to the
      expanded size. Note that the actual expanded size can be smaller if there is not enough
      space in the containing list. In that case the item becomes flickable automatically.
     */
    property real expandedHeight: collapsedHeight

    /*!
      If set to true, the item will collapse again when the user clicks somewhere in the always
      visible (when collapsed) area.
     */
    property bool collapseOnClick: false

    /*!
      Reparent any content to inside the Flickable
      \qmlproperty QtObject children
      \default
     */
    default property alias children: flickableContent.data

    /*! \internal */
    QtObject {
        id: priv

        /*!
          \internal
          Points to the containing ExpandablesListView or ExpandablesColumn
         */
        property Item view: root.ListView.view ? root.ListView.view : (root.parent.parent.parent.hasOwnProperty("expandItem") ? root.parent.parent.parent : null)

        /*! \internal
          Gives information whether this item is inside a ExpandablesListView or ExpandablesColumn
         */
        readonly property bool isInExpandableList: view && view !== undefined && view.hasOwnProperty("expandItem") && view.hasOwnProperty("collapse")

        /*! \internal
          Gives information if there is another item expanded in the containing ExpandablesListView or ExpandablesColumn
         */
        readonly property bool otherExpanded: isInExpandableList && view.expandedItem !== null && view.expandedItem !== undefined && view.expandedItem !== root

        /*! \internal
          Gives information about the maximum expanded height, in case that is limited by the containing ExpandablesListView or ExpandablesColumn
         */
        readonly property real maxExpandedHeight: isInExpandableList ? Math.min(view.height - collapsedHeight, expandedHeight) : expandedHeight
    }

    states: [
        State {
            name: ""
            PropertyChanges { target: root; opacity: 1 }
        },
        State {
            name: "otherExpanded"; when: priv.otherExpanded
            PropertyChanges { target: root; opacity: .5 }
        },
        State {
            name: "expanded"; when: expanded
            PropertyChanges { target: root; z: 3 }
        }
    ]

    /*! \internal */
    onExpandedChanged: {
        if (!expanded) {
            contentFlickable.contentY = 0;
        }

        if (priv.isInExpandableList) {
            if (expanded) {
                priv.view.expandItem(root);
            } else {
                priv.view.collapse();
            }
        }
    }

    Behavior on height {
        UbuntuNumberAnimation {}
    }
    Behavior on opacity {
        UbuntuNumberAnimation {}
    }

    Flickable {
        id: contentFlickable
        objectName: "__expandableContentFlickable"
        anchors { fill: parent; leftMargin: root.__contentsMargins; rightMargin: __contentsMargins; bottomMargin: divider.height }
        interactive: root.expanded && contentHeight > height + root.divider.height
        contentHeight: root.expandedHeight
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Behavior on contentY {
            UbuntuNumberAnimation {}
        }

        Item {
            id: flickableContent
            anchors {
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height
        }
    }

    MouseArea {
        anchors { left: parent.left; top: parent.top; right: parent.right }
        enabled: root.collapseOnClick && root.expanded
        height: root.collapsedHeight
        onClicked: root.expanded = false;
    }
}
