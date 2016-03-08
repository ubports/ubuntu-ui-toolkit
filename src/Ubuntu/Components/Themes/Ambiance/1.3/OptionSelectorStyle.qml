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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: ambianceStyle
    objectName: "OptionSelector13"

    property url chevron: Qt.resolvedUrl("../artwork/chevron_down.png")
    property url tick: Qt.resolvedUrl("../artwork/tick.png")
    property bool colourComponent: true
    property alias content: backgroundFrame

    UbuntuShape {
        id: background

        width: styledItem.width
        height: styledItem.height
        radius: "medium"
        source: shapeSource
    }

    ShaderEffectSource {
        id: shapeSource
        sourceItem: backgroundFrame
        hideSource: true
        // FIXME: visible: false prevents rendering so make it a nearly
        // transparent 1x1 pixel instead
        opacity: 0.01
        width: 1
        height: 1
    }

    Rectangle {
        id: backgroundFrame
        anchors.fill: parent
        color: styledItem.enabled
                ? theme.palette.normal.background
                : theme.palette.disabled.background
    }
}
