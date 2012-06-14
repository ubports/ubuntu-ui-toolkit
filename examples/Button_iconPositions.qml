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

    width: 800
    height: 300

    property bool showText: true
    property bool showIcon: true
    property bool iconLeft: true

    Button {
        id: button
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        iconSource: (showIcon) ? "call_icon.png" : ""
        text: (showText) ? "A few words of text": ""
        iconPosition: (iconLeft) ? "left" : "right"
        width: 250
        height: 80
        onClicked: print("click!")
    }

    Row {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        Button {
            text: (showText) ? "Remove text" : "Add text"
            width: 180
            onClicked: showText = !showText
        }
        Button {
            text: (showIcon) ? "Remove icon" : "Add icon"
            width: 180
            onClicked: showIcon = !showIcon
        }
        Button {
            text: (iconLeft) ? "Icon right" : "Icon left"
            width: 180
            onClicked: iconLeft = !iconLeft
            enabled: (showText && showIcon)
        }
    }
} // window
