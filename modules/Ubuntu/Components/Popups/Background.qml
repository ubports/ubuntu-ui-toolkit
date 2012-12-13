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

// internal popup background
Item {
    id: background
    anchors.fill: parent

    // parent must exist and be an instance of PopupBase
    property alias base: background.parent

    // show a semi-transparent dark Rectangle that dims
    // what is behind the background
    property bool dim: true

    // When this property is set, clicks anywhere on the background
    // will hide the popup, and the mouse event are forwarded by
    // setting mouse.accepted to false.
    property bool dismissOnTap

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.6
        visible: background.dim
    }

    MouseArea {
        anchors.fill: parent
        onPressed: if (background.dismissOnTap) base.hide()
    }
}
