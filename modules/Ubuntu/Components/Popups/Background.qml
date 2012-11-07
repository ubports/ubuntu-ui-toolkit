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

Item {
    id: background
    anchors.fill: parent
    property alias base: background.parent

    property bool dim: true

    // the names "transient" and "volatile" are reserved words
    property bool ephemeral

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.6
        visible: background.dim
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (ephemeral) {
                base.hide();
                mouse.accepted = false;
            } else {
                mouse.accepted = true;
            }
        }
    }
}
