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
        with a label (text), and optionally an icon, a progression arrow,
        and it can have an embedded Item (\l control) that can be used
        for including Buttons, Switches etc. inside the list item.

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
            }
        }
    \endqml
    \b{This component is under heavy development.}
*/
Empty {
    id: listItem
    height: 48

    /*!
      \preliminary
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: label.text

    /*!
      \preliminary
      The location of the icon to show in the list item (optional).
      \qmlproperty url iconSource
     */
    property alias iconSource: controlArea.iconSource

    /*!
      \preliminary
      Show or hide the frame around the icon
      \qmlproperty bool iconFrame
     */
    property alias iconFrame: controlArea.iconFrame

    /*!
      \preliminary
      An optional control that is displayed inside the list item.
      The width of the control must be specified in order to determine
      the layout of the list item.

      The mouseArea of the control will be set to the full Standard list item if
      there is no \l progression, or only the part left of the split, if there is a
      \l progression.
    */
    property alias control: controlContainer.control

    /*!
      \preliminary
      Show or hide the progression symbol.
     */
    property bool progression: false

    /*!
      \internal
     */
    property bool __showSplit: (control !== null) && progression

    // Children will take care of highlighting
    __highlighted: false

    Base {
        id: controlArea

        __showTopSeparator: false
        __showBottomSeparator: false

        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
            right: progressionItem.left
        }

        // if __showSplit, use the control's onClicked to define
        // the action when clicking on the controlArea.
        onClicked: if (!__showSplit) listItem.clicked()

        selected: listItem.selected
        progression: listItem.progression && !listItem.__showSplit
        LabelVisual {
            id: label
            selected: listItem.selected
            anchors {
                verticalCenter: parent.verticalCenter
                leftMargin: 5
                left: parent.left
                right: controlContainer.left
            }
        }
        Item {
            id: controlContainer
            property Item control
            width: control ? control.width : 0
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                margins: 5
            }
            onControlChanged: {
                control.parent = controlContainer;
                if (control.hasOwnProperty("mouseArea")) control.mouseArea = controlArea.mouseArea;
            }
        }
    }

    Empty {
        id: progressionItem
        visible: listItem.__showSplit
        width: visible ? progressionHelper.width : 0
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        __showTopSeparator: false
        __showBottomSeparator: false

        onClicked: listItem.clicked()

        ProgressionVisual {
            id: progressionHelper
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }
            showSplit: true
        }
    }
}
