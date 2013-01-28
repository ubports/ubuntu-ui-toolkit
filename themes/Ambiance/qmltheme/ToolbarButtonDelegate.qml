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

    implicitWidth: units.gu(6)
    implicitHeight: units.gu(6)

    Image {
        id: icon
        anchors {
            top: parent.top
            topMargin: units.gu(1)
            horizontalCenter: parent.horizontalCenter
        }
        width: units.gu(2)
        height: units.gu(2)
        source: item.iconSource
        opacity: button.enabled ? 1.0 : 0.3
    }

    Label {
        id: label
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: units.gu(1)
        }
        width: paintedWidth
        text: item.text
        fontSize: "x-small"
        color: "#888888"
        opacity: button.enabled ? 1.0 : 0.3
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
        opacity: 0.4
        visible: button.pressed
    }
}
