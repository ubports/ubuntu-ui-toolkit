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
import Ubuntu.Components 0.1

/*!
    \qmlclass Empty
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief A list item with no contents.
    The Empty class can be used for generic list items, containing other
    components such as buttons. It is selectable, and can take mouse clicks.
    It will attempt to detect if a separator line at the bottom of the item
    is suitable, but this behaviour can be over-ridden (using \l showSeparator).
    For specific types of list items, see its subclasses.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.Empty {
                height: 50
                Text {
                    text: "Hello world"
                    anchors.centerIn: parent
                }
            }
            ListItem.Empty {
                height: 100
                Tabs {
                    height: parent.height - 20
                    width: parent.width - 20
                    anchors.centerIn: parent
                    Tab {
                        text: "Tab 1"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page one"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 2"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page two"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 3"
                        page: TextCustom {
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
    width: parent ? parent.width : 250

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
        color: "white"
        opacity: 0.7
    }

    /*!
      \preliminary
      Set to show or hide the bottom separator line. This line is shown by default
      except in cases where this item is the delegate of a ListView.
     */
    property bool showSeparator: __showSeparator()

    /*!
      \internal
      Method to automatically determine if the bottom separator line should be drawn.
      This always returns true, unless item is a delegate in a ListView. If in a ListView
      it will return false only when:
       + if there is a section.delegate below this item (as separator line and section
         delegate should not both appear)
       + if this is the final item in the list, and ListView.footer is set (again as
         separator line won't look well with footer below it)
     */
    function __showSeparator() {
        // if we're not in ListView, always show bottom separator line
        if (ListView.view !== null) {

            // if we're last item in ListView, show separator if no footer is defined
            // and hide it if footer defined
            if (index === ListView.view.model.count - 1) return !ListView.footer;

            // if section.delegate is positioned between this item and the next
            else if (ListView.section !== ListView.nextSection) return false;
        }
        return true;
    }

    ItemSeparator {
        id: bottomSeparatorLine
        anchors.bottom: parent.bottom
        visible: showSeparator
    }

    /*!
      \internal
      Reparent so that the visuals of the children do not
      occlude the separator lines.
     */
    default property alias children: body.children
    Item {
        id: body
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: bottomSeparatorLine.top
        }
    }
}
