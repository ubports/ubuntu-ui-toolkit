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
import "../Components"

Rectangle {
    id: window

    width: 600
    height: 400

    property bool showText: true
    property bool showIcon: true
    property bool iconLeft: true

    property string iconSrc: (showIcon) ? "call_icon.png" : ""
    property string iconPos: (iconLeft) ? "left" : "right"

    Column {
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        Row {
            Button {
                iconSource: iconSrc
                text: (showText) ? "Default" : ""
                iconPosition: iconPos
                width: 180
                height: 50
                onClicked: print("Clicked default button")
            }
            Button {
                iconSource: iconSrc
                text: (showText) ? "Green" : ""
                iconPosition: iconPos
                color: "#37b301"
                width: 180
                height: 50
                onClicked: print("Clicked green button")
            }
        } // Row
        Row {
            Button {
                iconSource: iconSrc
                text: (showText) ? "Disabled" : ""
                enabled: false
                iconPosition: iconPos
                width: 180
                height: 50
                onClicked: print("Clicked disabled button")
            }
            Button {
                iconSource: iconSrc
                text: (showText) ? "Press me" : ""
                iconPosition: iconPos
                pressedColor: "#dd4f22"
                width: 180
                height: 50
                onClicked: print("Clicked press-me button")
            }
        } // Row
        Row {
            Button {
                iconSource: iconSrc
                text: (showText) ? "Small blue text" : ""
                textSize: "small"
                textColor: "blue"
                iconPosition: iconPos
                width: 180
                height: 50
                onClicked: print("Clicked small-text button")
            }
            Button {
                iconSource: iconSrc
                text: (showText) ? "Larger text" : ""
                textSize: "x-large"
                textColor: "red"
                iconPosition: iconPos
                width: 180
                height: 50
                onClicked: print("Clicked large-text button")
            }
        } // Row
    } // Column

    Row {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        Button {
            text: (showText) ? "Remove text" : "Add text"
            width: 120
            color: "#448"
            onClicked: showText = !showText
            textSize: "large"
        }
        Button {
            text: (showIcon) ? "Remove icon" : "Add icon"
            width: 120
            color: "#448"
            onClicked: showIcon = !showIcon
        }
        Button {
            text: (iconLeft) ? "Icon right" : "Icon left"
            width: 120
            color: "#448"
            onClicked: iconLeft = !iconLeft
            enabled: (showText && showIcon)
        }
    }
} // window
