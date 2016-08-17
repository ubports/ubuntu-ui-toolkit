/*
 * Copyright 2015 Canonical Ltd.
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

// Test if the UbuntuShape is correctly updated when its source is a ShaderEffectSource.

import QtQuick 2.4
import Ubuntu.Components 1.2

Rectangle {
    id: root
    width: 300
    height: 150
    color: "#c0c0c0"

    Text {
        id: text
        anchors.centerIn: parent
        font.family: "Ubuntu"
        font.pixelSize: 34
        color: "#c0c0c0"
        text: "This is a scrolling text... "
        font.bold: true
    }

    ShaderEffectSource {
        id: effectSource
        sourceItem: text
        hideSource: true
        visible: false
    }

    UbuntuShape {
        id: shape
        anchors.fill: parent
        anchors.margins: 50
        backgroundColor: "black"
        radius: "medium"
        source: effectSource
        sourceFillMode: UbuntuShape.Pad
        sourceHorizontalAlignment: UbuntuShape.AlignLeft
        sourceHorizontalWrapMode: UbuntuShape.Repeat

        PropertyAnimation on sourceTranslation {
            from: Qt.vector2d(0.0, 0.0)
            to: Qt.vector2d(-text.width / shape.width, 0.0)
            loops: Animation.Infinite
            duration: 2500
        }
    }
}
