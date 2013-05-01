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
    id: visuals
    // styling properties
    property color color
    property real barOpacity

    anchors.fill: parent
    property alias contentItem: background

    Rectangle {
        id: background
        anchors.fill: parent
        color: visuals.color
        opacity: visuals.barOpacity
    }

    Image {
        id: dropshadow
        anchors {
            left: parent.left
            right: parent.right
            bottom: background.top
        }
        source: Qt.resolvedUrl("artwork/toolbar_dropshadow.png")
        opacity: item.active || item.animating ? 0.5 : 0.0
        Behavior on opacity {
            NumberAnimation { duration: 50; easing.type: Easing.OutQuad }
        }
    }
}
