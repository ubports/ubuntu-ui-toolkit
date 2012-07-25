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
    \qmlclass Standard
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief The standard list item class. It shows a basic list item
        with a label (text), and optionally an icon and a progression arrow.

    Examples:
    \qml
        import Ubuntu.Components.ListItems 0.1 as ListItem
        ListItem.Container {
            ListItem.Standard {
                text: "Selectable standard list item"
                selected: false
                onClicked: selected = !selected
               }
            ListItem.Standard {
                text: "List item with icon"
                iconSource: "icon.png"
            }
            ListItem.Standard {
                text: "With a progression arrow"
                progression: true
            }
        }
    \endqml
    \b{This component is under heavy development.}
*/
Selectable {
    id: listItem
    height: 54

    /*!
      \preliminary
      The location of the icon to show in the list item (optional).
      \qmlproperty url iconSource
     */
    property alias iconSource: iconHelper.source

    /*!
      \preliminary
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: label.text

    /*!
      \preliminary
      Show or hide the progression symbol.
     */
    property bool progression: false

    // TODO: doc
    property alias control: controlContainer.control

    IconVisual {
        id: iconHelper
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
    }
    LabelVisual {
        id: label
        selected: listItem.selected
        anchors {
            verticalCenter: parent.verticalCenter
            leftMargin: 5
            left: iconHelper.right
//            right: progressionHelper.left
            right: controlContainer.left
        }
    }
    Item {
        id: controlContainer
        property Item control
//        visible: control ? true : false
        //height: parent.height
        width: control ? control.width : 0
//        height: 30
//        Rectangle { color: "yellow"; anchors.fill: controlContainer; z: -1 }
        anchors {
//            left: label.right
            right: progressionHelper.left
            leftMargin: 5
            rightMargin: 5
            top: parent.top
            bottom: parent.bottom
            topMargin: 5
            bottomMargin: 5
        }
        onControlChanged: control.parent = controlContainer
    }
    ProgressionVisual {
        id: progressionHelper
        visible: listItem.progression
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        showSplit: control ? true : false
    }
}
