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
    anchors.fill: parent
    property alias contentItem: contents

//    z: -1
    Rectangle {
//        z: -1
        id: background
//        anchors {
//            left: parent.left
//            right: parent.right
//            bottom: parent.bottom
//        }
//        height: parent.height //- dropshadow.height
        anchors.fill: parent
        color: StyleUtils.itemStyleProperty("color")
        opacity: StyleUtils.itemStyleProperty("opacity")

        Item {
            id: contents
            anchors.fill: parent
        }
    }



    Image {
        id: dropshadow
        anchors {
            left: parent.left
            right: parent.right
            bottom: background.top
        }
        source: Qt.resolvedUrl("artwork/toolbar_dropshadow.png")
        opacity: item.state == "" ? 0.0 : 0.5
        Behavior on opacity {
            NumberAnimation { duration: 50; easing.type: Easing.OutQuad }
        }
    }
}
