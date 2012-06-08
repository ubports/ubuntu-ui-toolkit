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
    width: 800
    height: 600

    color: "#cccccc"

    Row {
        anchors.fill: parent
        anchors.margins: 50
        spacing: 50

        Column {
            width: 200
            spacing: 20

            Button {
                text: "Send"
            }

            Button {
                text: "Send"
                enabled: false
            }

            Button {
                iconSource: "call_icon.png"
                color: "#37b301"
            }

            Button {
                text: "Edit"
                pressedColor: "#dd4f22"
            }

            Button {
                width: 150
                text: "Send"
            }

            Button {
                width: 200
                height: 80
                text: "Send"
            }
        }

        Column {
            width: 200
            spacing: 5

            Button {
                text: "Call"
                iconSource: "call_icon.png"
                iconPosition: "left"
                color: "#37b301"
            }
            Button {
                text: "Call"
                iconSource: "call_icon.png"
                iconPosition: "right"
                color: "#37b301"
            }
        }
    }
}
