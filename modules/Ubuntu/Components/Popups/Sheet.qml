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

import QtQuick 2.0
import Ubuntu.Components 0.1

PopupBase {
    id: sheet
    default property alias container: containerItem.data

    property alias title: headerText.text

    // choose between "closeButton", "doneButton", "composerButtons"
    property string buttonConfiguration: "closeButton"

    Background {
        dim: false
        ephemeral: false
    }

    Foreground {

        width: MathUtils.clamp(childrenRect.width, units.gu(50), sheet.width)
        height: Math.min(units.gu(40), sheet.height)

        Rectangle {
            id: header
            color: "darkgrey"
            height: units.gu(8)
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            Button {
                ItemStyle.class: "transparent-button"
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(1)
                }
                width: height
                text: "X"
                onClicked: sheet.hide()
                visible: "closeButton" === sheet.buttonConfiguration
            }
            Button {
                anchors {
                    left:  parent.left
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(1)
                }
                text: "cancel"
                color: "grey"
                onClicked: sheet.hide()
                visible: "composerButtons" === sheet.buttonConfiguration
            }

            TextCustom {
                id: headerText
                anchors {
                    centerIn: parent
                }
            }
            Button {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: units.gu(1)
                }
                text: "done"
                color: "orange"
                onClicked: sheet.hide()
                visible: "doneButton" === sheet.buttonConfiguration
            }
            Button {
                anchors {
                    right:  parent.right
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(1)
                }
                text: "confirm"
                color: "orange"
                onClicked: sheet.hide()
                visible: "composerButtons" === sheet.buttonConfiguration
            }
        }

        // TODO: Make height of Foreground depend on containerItem height + margins?
        // TODO: make item after testing.
        Rectangle {
            id: containerItem
            color: "transparent"

            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }

            height: childrenRect.height + anchors.margins //anchors.topMargin + anchors.bottomMargin
        }
    }
}
