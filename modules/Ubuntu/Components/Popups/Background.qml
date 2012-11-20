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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

// internal popup background
Item {
    id: background
    anchors.fill: parent
    Theming.ItemStyle.class: "popup-background"

    // parent must exist and be an instance of PopupBase
    property alias base: background.parent

    // show a semi-transparent dark Rectangle that dims
    // what is behind the background
    property bool dim: true

    // The names "transient" and "volatile" are reserved words.
    // When this property is set, clicks anywhere on the background
    // will hide the popup, and the mouse event are forwarded by
    // setting mouse.accepted to false.
    property bool ephemeral

    Rectangle {
        anchors.fill: parent
//        color: "black"
        // FIXME: see above
        color: Theming.ItemStyle.style && Theming.ItemStyle.style.color ? Theming.ItemStyle.style.color : "yellow"
//        color: Theming.ItemStyle.style.color
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
