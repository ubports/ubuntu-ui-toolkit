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
                icon: Qt.resolvedUrl("icon.png")
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
      The location of the icon to show in the list item (optional), or an Item that is
      shown on the left side inside the list item. The icon will automatically be
      anchored to the left side of the list item, and if its height is undefined, to the top
      and bottom of the list item with all margins 5.
      \qmlproperty variant icon
    */
    property variant icon

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
      The width of the control must be specified in order to determine
      the layout of the list item.

      The mouseArea of the control will be set to the full Standard list item if
      there is no \l progression, or only the part left of the split, if there is a
      \l progression.
      \qmlproperty Item control
    */
    property alias control: controlContainer.control

    // If there is a control, the controlArea covers the listItem's mouseArea,
    // so in that case use the highlights below when pressed
    highlightWhenPressed: !listItem.control

    Rectangle {
        id: controlHighlight
        visible: controlArea.pressed
        anchors.fill: controlArea
        color: "white"
        opacity: 0.7
    }
    Rectangle {
        id: progressionHighlight
        visible: listItem.pressed
        anchors.fill: progressionHelper
        color: "white"
        opacity: 0.7
    }

    /*!
      \internal
      The internal representation of the icon. Can be equal to \l icon if that is
      an item, or an initialized IconVisual if \l icon is a url.
     */
    property Item __leftItem
    onIconChanged: {
        if (__leftItem) {
            __leftItem.visible = false;
            if (__leftItem.pleaseCleanMeUp === true) __leftItem.destroy();
            __leftItem = null;
        }

        if (typeof icon == "string") {
            // icon is the url of an image
            __leftItem = Qt.createQmlObject('IconVisual { source: "'+icon+'"; __pleaseCleanMeUp: true }', listItem);
        } else {
            __leftItem = icon;
            __leftItem.parent = listItem;
        }

        __leftItem.anchors.left = listItem.left;
        if (!__leftItem.height) {
            __leftItem.anchors.top = listItem.top;
            __leftItem.anchors.bottom = listItem.bottom;
            __leftItem.anchors.margins = 5;
        }
    }

    LabelVisual {
        id: label
        selected: listItem.selected
        anchors {
            verticalCenter: parent.verticalCenter
            leftMargin: __leftItem ? __leftItem.width + __leftItem.anchors.leftMargin + __leftItem.anchors.rightMargin + 5 : 5
            left: parent.left
            right: controlContainer.left
        }
    }
    Item {
        id: controlContainer
        property Item control
        // use the width of the control if there is (possibly elided) text,
        // or full width available if there is no text.
        width: control ? control.width : undefined
        anchors {
            right: progressionHelper.left
            top: parent.top
            bottom: parent.bottom
            margins: 5
        }
        onControlChanged: {
            control.parent = controlContainer;
            if (control.hasOwnProperty("mouseArea")) control.mouseArea = controlArea;
        }
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
