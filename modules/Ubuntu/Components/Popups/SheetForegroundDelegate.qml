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

Item {
    width: item.contentsWidth
    height: header.height + containerItem.height

    property alias contentItem: containerItem

    // Avoid mouse events being sent to any MouseAreas that are behind the sheet
    MouseArea {
        anchors.fill: parent
    }

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
            text: item.title
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

        Connections {
            target: item
            onLeftButtonChanged: header.updateButton(item.leftButton, leftButtonContainer)
            onRightButtonChanged: header.updateButton(item.rightButton, rightButtonContainer)
        }
    }

    Rectangle {
        id: containerItem
        color: "lightgrey"
        height: MathUtils.clamp(item.contentsHeight, item.minHeight - header.height, item.maxHeight - header.height)
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
        }
    }
}
