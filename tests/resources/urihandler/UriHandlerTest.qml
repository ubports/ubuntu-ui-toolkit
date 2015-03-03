/*
 * Copyright 2013 Canonical Ltd.
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
 *
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 1.1

Item {
    id: mainView
    width: 700
    height: 100

    Rectangle {
        id: background
        anchors.fill: parent
        color: "black"
    }

    Text {
        id: uri
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 20
        font.bold: true
        font.family: "Ubuntu"
        color: "white"
        text: "Uri: \"\""
    }

    Connections {
        target: UriHandler
        onOpened: uri.text = "Uri: \"" + uris + "\""
    }
}
