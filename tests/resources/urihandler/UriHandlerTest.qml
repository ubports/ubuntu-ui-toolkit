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
import Ubuntu.Components 0.1

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
        id: path
        width: parent.width
        height: parent.height / 2.0
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 20
        font.bold: true
        font.family: "Ubuntu"
        color: "white"
        text: "Object path: \"\""
    }

    Text {
        id: uri
        y: parent.height / 2.0
        width: parent.width
        height: parent.height / 2.0
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 20
        font.bold: true
        font.family: "Ubuntu"
        color: "white"
        text: "Uri[0]: \"\""
    }

    Component.onCompleted: UriHandler.init();
    Connections {
        target: UriHandler
        onObjectPathChanged: path.text = "Object path: \"" + UriHandler.objectPath + "\""
        onOpened: uri.text = "Uri[0]: \"" + uris[0] + "\""
    }
}
