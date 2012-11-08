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
        property real contentsWidth: Math.max(header.width, containerItem.width)
        width: MathUtils.clamp(contentsWidth, units.gu(50), sheet.width)
        height: Math.min(units.gu(40), sheet.height)

        y: units.gu(15)
        anchors.horizontalCenter: parent.horizontalCenter

        onWidthChanged: print("width = "+width)
        Component.onCompleted: print("w = "+sheet.width)

        Rectangle {
            id: header
            color: "darkgrey"
            height: units.gu(8)
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            width: headerText.width + totalButtonWidth()

            TextCustom {
                id: headerText
                anchors {
                    centerIn: parent
                }
                width: headerText.implicitWidth + units.gu(4)
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
//                wrapMode: Text.Wrap
//                maximumLineCount: 2
            }

            function totalButtonWidth() {
                var total = 0;
                if (closeButton.visible) total += 2*closeButton.width;
                if (doneButton.visible) total += 2*doneButton.width;
                if (cancelButton.visible) total += 2*Math.max(cancelButton.width, confirmButton.width)
//                if (confirmButton.visible) total += confirmButton.width;
                return total;
            }

            Button {
                id: closeButton
                ItemStyle.class: "transparent-button"
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(1)
                }
                text: "X"
                onClicked: sheet.hide()
                visible: "closeButton" === sheet.buttonConfiguration
            }
            Button {
                id: cancelButton
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
            Button {
                id: doneButton
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
                id: confirmButton
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
//                left: parent.left
//                right: parent.right
                horizontalCenter: parent.horizontalCenter
//                margins: units.gu(2)
            }

            height: childrenRect.height //+ anchors.margins //anchors.topMargin + anchors.bottomMargin
            width: childrenRect.width
        }
    }
}
