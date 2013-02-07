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
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        top: parent ? parent.top : undefined
    }
    property real totalHeight: headerText.totalHeight + questionText.totalHeight + columnContainer.height
    height: MathUtils.clamp(totalHeight, item.minHeight, item.maxHeight)
    property alias contentItem: columnContainer

    Rectangle {
        id: container
        color: StyleUtils.itemStyleProperty("color", "white")
        anchors.fill: parent

        Label {
            id: headerText
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            property real totalHeight: height + 2*anchors.margins
            fontSize: "large"
            horizontalAlignment: Text.AlignHCenter
            color: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            text: item.title
        }

        Label {
            id: questionText
            anchors {
                top: headerText.bottom
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            property real totalHeight: height + 2*anchors.margins
            width: parent.width - 2*anchors.margins
            fontSize: "medium"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            color: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            text: item.text
        }

        Item {
            id: columnContainer
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
            height: childrenRect.height
        }
    }

    clip: true // hide the ShaderEffectSource
    Shape {
        anchors.fill: parent
        image: effectSource
        radius: StyleUtils.itemStyleProperty("radius", "small")
        borderSource: Qt.resolvedUrl("artwork/ubuntushape_"+radius+"_radius_idle.sci")
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
