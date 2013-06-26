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
    id: visuals
    // styling properties
    visible: styledItem.dismissArea.width > units.gu(60)
    property color color: visible ? Qt.rgba(0, 0, 0, 0.7) : "transparent"
    property string radius: "medium"

    anchors.fill: parent
    z: -1

    Rectangle {
        id: container
        color: visuals.color
        anchors.fill: parent
        radius: 10
    }

    clip: true // hide the ShaderEffectSource
    Shape {
        anchors.fill: parent
        image: effectSource
        radius: visuals.radius
        borderSource: Qt.resolvedUrl("artwork/ubuntushape_"+radius+"_radius_mask.sci")
    }

    ShaderEffectSource {
        smooth: false // prevent linear interpolation
        id: effectSource
        hideSource: true
        sourceItem: container
        format: ShaderEffectSource.RGBA
        live: true

        // Do not set visible to false because it will leave the FBO empty,
        //  but position the ShaderEffectSource somewhere that it will be clipped
        //  so it is not visible.
        x: width
        width: sourceItem.width
        height: sourceItem.height
    }
}
