/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.0
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

AbstractButton {
    id: button
    property alias text: label.text
    property alias icon: icon.source

    Theming.ItemStyle.class: "transparent"
    width: units.gu(6)
    height: units.gu(6)

    Image {
        id: icon
        anchors.top: parent.top
        anchors.topMargin: units.gu(1)
        anchors.horizontalCenter: parent.horizontalCenter
        height: units.gu(2)
        width: units.gu(2)
        opacity: button.enabled ? 1.0 : 0.3
    }

    Label {
        id: label
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: units.gu(1)
        fontSize: "x-small"
        color: "#888888"
        width: paintedWidth
        opacity: button.enabled ? 1.0 : 0.3
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
        opacity: 0.4
        visible: button.pressed
    }
}
