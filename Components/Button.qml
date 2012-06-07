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
    \qmlclass Button
    \inqmlmodule UbuntuUIToolkit
    \brief The Button class adds an icon and text to the AbstractButton,
            as well as a background for the button using ButtonBackground.

    \b{This component is under heavy development.}

    A Button can have text, an icon, or both.
    The background of the button can change depending on the Button's state.

    Examples:
    \qml
        Column {
            width: 155
            spacing: 5

            Button {
                text: "text only (centered)\nwith border"
                onClicked: print("clicked text-only Button")
            }
            Button {
                iconSource: "call_icon.png"
                onClicked: print("clicked icon-only Button")
                iconPosition: "top"
                color: "green"
            }
            Button {
                iconSource: "call_icon.png"
                text: "Icon on right"
                iconPosition: "right"
                onClicked: print("clicked on Button with text and icon")
            }
        }
    \endqml
*/
AbstractButton {
    id: button

    /*!
      \preliminary
      The dimensions of the button.
    */
    width: 150
    height: 50

    /*!
       \preliminary
       The source URL of the icon to display inside the button.
       Leave this value blank for a text-only button.
       \qmlproperty url iconSource
    */
    property alias iconSource: icon.source

    /*!
       \preliminary
       The text to display in the button. If an icon was defined,
       the text will be shown next to the icon, otherwise it will
       be centered. Leave blank for an icon-only button.
       \qmlproperty string text
    */
    property alias text: label.text

    /*!
      \preliminary
      The size of the text that is displayed in the button.
      \qmlproperty string textSize
    */
    property alias textSize: label.fontSize

    /*!
      \preliminary
      The color of the text.
      \qmlproperty color textColor
    */
    property alias textColor: label.color

    /*!
       \preliminary

       The position of the icon relative to the text.
       top, bottom, left or right.
       If only text or only an icon is defined, this
       property is ignored and the text or icon is
       centered horizontally and vertically in the button.

       Currently this is a string value. We are waiting for
       support for enums:
       https://bugreports.qt-project.org/browse/QTBUG-14861
    */
    property string iconPosition: "left"

    /*
      \preliminary
      Give the button a background, which is a Rectangle.
     */
    property ButtonBackground background: ButtonBackground { parent: button }

    Image {
        id: icon
        fillMode: Image.PreserveAspectFit
        anchors.margins: 10
        height: {
            if (text===""||iconPosition=="left"||iconPosition=="right") return button.height - 20;
            else return button.height - label.implicitHeight - 30;
        }
     }

    TextCustom {
        id: label
        anchors.margins: 10
        fontSize: "medium"
    }

    Item { //placed in here to keep state property private
        id: positioner

        states: [
            State {
                name: "top"
                AnchorChanges { target: icon; anchors {top: button.top; horizontalCenter: button.horizontalCenter}}
                AnchorChanges { target: label; anchors {top: icon.bottom; horizontalCenter: button.horizontalCenter}}
            },
            State {
                name: "bottom"
                AnchorChanges { target: icon; anchors {bottom: button.bottom; horizontalCenter: button.horizontalCenter}}
                AnchorChanges { target: label; anchors {bottom: icon.top; horizontalCenter: button.horizontalCenter}}
            },
            State {
                name: "right"
                AnchorChanges { target: icon; anchors {right: button.right; verticalCenter: button.verticalCenter}}
                AnchorChanges { target: label; anchors {right: icon.left; verticalCenter: button.verticalCenter}}
            },
            State {
                name: "left"
                AnchorChanges { target: icon; anchors {left: button.left; verticalCenter: button.verticalCenter}}
                AnchorChanges { target: label; anchors {left: icon.right; verticalCenter: button.verticalCenter}}
            },
            State {
                name: "center"
                AnchorChanges { target: icon; anchors {horizontalCenter: button.horizontalCenter; verticalCenter: button.verticalCenter}}
                AnchorChanges { target: label; anchors {horizontalCenter: button.horizontalCenter; verticalCenter: button.verticalCenter}}
            }
        ]
        state: (button.iconSource == "" || button.text == "") ? "center" : iconPosition
    }
}
