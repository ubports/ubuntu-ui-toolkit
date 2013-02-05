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

import QtQuick 2.0
import Ubuntu.Components 0.1

/*!
    \qmltype Empty
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief A list item with no contents.
    The Empty class can be used for generic list items, containing other
    components such as buttons. It is selectable, and can take mouse clicks.
    It will attempt to detect if a thin dividing line at the bottom of the
    item is suitable, but this behaviour can be over-ridden (using \l showDivider).
    For specific types of list items, see its subclasses.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.Empty {
                height: units.gu(6)
                Text {
                    text: "Hello world"
                    anchors.centerIn: parent
                }
            }
            ListItem.Empty {
                height: units.gu(13)
                Tabs {
                    height: parent.height - units.gu(3)
                    width: parent.width - units.gu(3)
                    anchors.centerIn: parent
                    Tab {
                        text: "Tab 1"
                        page: Label {
                            anchors.centerIn: parent
                            text: "Page one"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 2"
                        page: Label {
                            anchors.centerIn: parent
                            text: "Page two"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 3"
                        page: Label {
                            anchors.centerIn: parent
                            text: "Page three"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                }
            }
        }
    \endqml

    See the documentation of the derived classes of Empty for more examples.
    \b{This component is under heavy development.}
*/
AbstractButton {
    id: emptyListItem
    width: parent ? parent.width : units.gu(31)
    height: __height + bottomDividerLine.height

    /*!
      \preliminary
      Specifies whether the list item is selected.
     */
    property bool selected: false

    /*!
      \preliminary
      Highlight the list item when it is pressed.
      This is used to disable the highlighting of the full list item
      when custom highlighting needs to be implemented (for example in
      ListItem.Standard which can have a split).
    */
    property bool highlightWhenPressed: true

    Rectangle {
        id: highlight
        visible: emptyListItem.selected || (emptyListItem.highlightWhenPressed && emptyListItem.pressed)
        anchors.fill: parent
        color: "#E6E6E6"
        opacity: 0.7
    }

    /*!
      \internal
      Defines the height of the ListItem, so correct height of this component, including divider
      line is calculated.
     */
    property int __height: units.gu(6)

    /*!
      \preliminary
      Set to show or hide the thin bottom divider line (drawn by the \l ThinDivider component).
      This line is shown by default except in cases where this item is the delegate of a ListView.
     */
    property bool showDivider: __showDivider()

    /*!
      \internal
      Method to automatically determine if the bottom divider line should be drawn.
      This always returns true, unless item is a delegate in a ListView. If in a ListView
      it will return false only when:
       + if there is a section.delegate below this item (as thin divider line and section
         delegate should not both appear)
       + if this is the final item in the list, and ListView.footer is set (again as thin
         divider line won't look well with footer below it)
     */
    function __showDivider() {
        // if we're not in ListView, always show a thin dividing line at the bottom
        if (ListView.view !== null) {

            // if we're last item in ListView, show divider if no footer is defined
            // and hide it if footer defined
            if (index === ListView.view.model.count - 1) return !ListView.footer;

            // if section.delegate is positioned between this item and the next
            // UPDATE: no longer needed with the new visuals, but may be updated again.
            // else if (ListView.section !== ListView.nextSection) return false;
        }
        return true;
    }

    ThinDivider {
        id: bottomDividerLine
        anchors.bottom: parent.bottom
        visible: showDivider
    }

    /*!
      \internal
      Reparent so that the visuals of the children does not
      occlude the bottom divider line.
     */
    default property alias children: body.children
    Item {
        id: body
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: bottomDividerLine.top
            margins: emptyListItem.__contentsMargins
        }
    }

    /*! \internal
      The spacing inside the list item.
     */
    property real __contentsMargins: units.gu(0)
}
