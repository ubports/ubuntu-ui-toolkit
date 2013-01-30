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

    implicitWidth: units.gu(10)
    implicitHeight: units.gu(10)

    Image {
        id: icon
        anchors {
            top: parent.top
            topMargin: units.gu(2)
            horizontalCenter: parent.horizontalCenter
        }
        width: units.gu(4)
        height: units.gu(4)
        source: item.iconSource
        opacity: item.enabled ? 1.0 : 0.3
    }

    Label {
        id: label
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: units.gu(2)
        }
        width: paintedWidth
        text: item.text
    }
}
