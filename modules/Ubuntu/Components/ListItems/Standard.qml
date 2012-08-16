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
    \qmlclass Standard
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief The standard list item class. It shows a basic list item
        with a label (text), and optionally an icon and a progression arrow.

    Examples:
    \qml
        import UbuntuComponents 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
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
            ListItem.Standard {
                text: "Control"
                control: Button {
                    text: "Click me"
                    width: 150
                    onClicked: print("Clicked")
                }
                progression: true
                onClicked: control.clicked()
            }
            ListItem.Standard {
                control: Button {
                    text: "Big control"
                    anchors.fill: parent
                }
            }
        }
    \endqml
    \b{This component is under heavy development.}
*/
Base {
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

    /*!
      \preliminary
      An optional control that is displayed inside the list item.
      If \l text is specified, then the width of the control must be fixed
      in order to determine the layout of the list item.
      If no \l text is given, the control's parent will fill the full
      space available inside the list item, taking into account a possible
      icon and progression, and the control may be anchored to fill its parent.
      \qmlproperty Item control.
      The mouseArea of the control will be set to the full Standard list item if
      there is no \l progression, or only the part left of the split, if there is a
      \l progression.
    */
    property alias control: controlContainer.control

    IconVisual {
        id: iconHelper
        anchors {
            left: parent.left
            top: parent.top
        }
    }
    LabelVisual {
        id: label
        selected: listItem.selected
        anchors {
            verticalCenter: parent.verticalCenter
            leftMargin: 5
            left: iconHelper.right
            right: controlContainer.left
        }
    }
    Item {
        id: controlContainer
        property Item control
        // use the width of the control if there is (possibly elided) text,
        // or full width available if there is no text.
        width: label.text ? childrenRect.width : undefined
        anchors {
            left: label.text ? undefined : iconHelper.right
            right: progressionHelper.left
            top: parent.top
            bottom: parent.bottom
            margins: 5
        }
        onControlChanged: {
            control.parent = controlContainer
            control.mouseArea = controlArea
        }
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
    MouseArea {
        id: controlArea
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: progressionHelper.left
        }
        enabled: control !== null
    }
}
