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

    property Button leftButton
    property Button rightButton

    onLeftButtonChanged: __buttonChanged(leftButton, leftButtonContainer)
    onRightButtonChanged: __buttonChanged(rightButton, rightButtonContainer)

    function __buttonChanged(button, container) {
        if (!button) return;
        button.parent = container;
        button.anchors.left = container.left;
        button.anchors.right = container.right;
        button.anchors.verticalCenter = container.verticalCenter;
        button.anchors.margins = units.gu(1);
    }

    Background {
        dim: false
        ephemeral: false
    }

    Foreground {
        color: "lightgrey"
        property real contentsWidth: Math.max(header.requestedWidth, containerItem.requestedWidth)
        width: MathUtils.clamp(contentsWidth, units.gu(50), sheet.width)
        property real contentsHeight: header.height + containerItem.height
        height: MathUtils.clamp(contentsHeight, units.gu(40), contentsHeight, sheet.height)

        y: units.gu(15)
        anchors.horizontalCenter: parent.horizontalCenter

        onWidthChanged: print("width = "+width)
        onHeightChanged: print("height = "+height)
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
            property real requestedWidth: headerText.width + leftButtonContainer.width + rightButtonContainer.width

            TextCustom {
                id: headerText
                anchors {
                    centerIn: parent
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
        }

        Item {
            id: containerItem
            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            height: childrenRect.height + 2*anchors.margins
            property real requestedWidth: childrenRect.width + 2*anchors.margins
        }
    }
}
