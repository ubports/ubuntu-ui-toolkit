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
    \brief The Button class adds an icon and text to the ButtonBehavior,
            as well as a background for the button using ButtonBackground.

    \b{This component is under heavy development.}

    A ButtonFace can have text, an icon, or both.
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
ButtonBehavior {
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
        fontSize: "medium"
    }

    // this variable ensures that the anchors of the icon and label are
    // automatically updated when button.iconPosition changes.
    /*!
      \internal
    */
    property int __anchorsDummy: __alignIconText(button.iconPosition, button.iconSource, button.text);

    /*!
       \internal
    */
    function __alignIconText(pos, src, txt) {
        label.anchors.margins = 10
        if (button.iconSource=="") {
            // no icon.
            label.anchors.centerIn = button;
        } else if (button.text=="") {
            icon.anchors.centerIn = button;
        } else if (iconPosition=="top") {
            icon.anchors.top = button.top;
            icon.anchors.horizontalCenter = button.horizontalCenter;
            label.anchors.top = icon.bottom;
            label.anchors.horizontalCenter = button.horizontalCenter;
        } else if (iconPosition=="bottom") {
            icon.anchors.bottom = button.bottom;
            icon.anchors.horizontalCenter = button.horizontalCenter;
            label.anchors.bottom = icon.top;
            label.anchors.horizontalCenter = button.horizontalCenter;
        } else if (iconPosition=="right") {
            icon.anchors.right = button.right;
            icon.anchors.verticalCenter = button.verticalCenter;
            label.anchors.right = icon.left;
            label.anchors.verticalCenter = button.verticalCenter;
        } else if (iconPosition=="left") {
            icon.anchors.left = button.left;
            icon.anchors.verticalCenter = button.verticalCenter;
            label.anchors.left = icon.right;
            label.anchors.verticalCenter = button.verticalCenter;
        } // if textlocation
        return 1;
    } // alignIconText
}
