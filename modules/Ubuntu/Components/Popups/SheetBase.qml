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

    property alias contentsWidth: foreground.contentsWidth
    property alias contentsHeight: foreground.contentsHeight

    property alias title: headerText.text

    property Button leftButton
    property Button rightButton

    onLeftButtonChanged: header.updateButton(leftButton, leftButtonContainer)
    onRightButtonChanged: header.updateButton(rightButton, rightButtonContainer)

    Background {
        dim: false
        ephemeral: false
    }

    Foreground {
        id: foreground

        property real contentsWidth: units.gu(64)
        property real contentsHeight: units.gu(40)

        y: Math.min(units.gu(15), (sheet.height - height)/2)
        anchors.horizontalCenter: parent.horizontalCenter

        property real minWidth: Math.min(units.gu(50), sheet.width)
        property real minHeight: Math.min(units.gu(40), sheet.height)

        width: MathUtils.clamp(contentsWidth, minWidth, sheet.width)
        height: MathUtils.clamp(contentsHeight + header.height, units.gu(40), sheet.height)

        color: "lightgrey"

        Rectangle {
            id: header
            color: "darkgrey"
            height: units.gu(8)
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            TextCustom {
                id: headerText
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: leftButtonContainer.right
                    right: rightButtonContainer.left
                }
                width: headerText.implicitWidth + units.gu(4)
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
            }
            Item {
                id: leftButtonContainer
                width: units.gu(14)
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
            }
            Item {
                id: rightButtonContainer
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: units.gu(14)
            }

            function updateButton(button, container) {
                if (!button) return;
                button.parent = container;
                button.anchors.left = container.left;
                button.anchors.right = container.right;
                button.anchors.verticalCenter = container.verticalCenter;
                button.anchors.margins = units.gu(1);
            }
        }

        Item {
            id: containerItem
            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: units.gu(1)
            }
        }
    }
}
