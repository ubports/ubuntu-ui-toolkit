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

//EvilUbuntuShape {
//UbuntuShape {

Item {
    id: frame

//    item: contentItem
    property alias contentItem: body

    clip: true
//    color: StyleUtils.itemStyleProperty("color", "white")
//    radius: StyleUtils.itemStyleProperty("radius", "small")

    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        top: parent ? parent.top : undefined
    }
    height: childrenRect.height

    Item {
        id: body
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: childrenRect.height
    }


    Shape {
        anchors.fill: parent
//        visible: shape.visible
        image: fbo
//        borderSource: shape.borderSource
        borderSource: Qt.resolvedUrl("artwork/ubuntushape_small_radius_idle.sci")
//        radius: shape.radius
        radius: "small"
    }

    ShaderEffectSource {
        smooth: false // prevent linear interpolation
        id: fbo
        objectName: "FBO"
        hideSource: true
//        sourceItem: contentsContainer
//        sourceItem:
        sourceItem: frame.contentItem
//        format: ShaderEffectSource.RGBA
        live: true

        // Do not set visible to false because it will leave the FBO empty,
        //  but position the ShaderEffectSource somewhere that it will be clipped
        //  so it is not visible.
        x: width
        width: parent.width
        height: parent.height
//        width: sourceItem ? sourceItem.width : 0
//        height: sourceItem ? sourceItem.height: 0

        Rectangle {
            id: background
            parent: frame.contentItem
            anchors.fill: parent ? parent: undefined
//            color: "transparent"
            color: StyleUtils.itemStyleProperty("color", "white")
        }
    }

}
