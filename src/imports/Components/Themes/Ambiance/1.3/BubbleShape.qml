/*
 * Copyright 2016 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: bubbleShape

    /*!
      Do not use an UbuntuShape but a Rectangle as the background of the BubbleShape.
     */
    property bool square: false

    /*!
      The background color of the bubble.
     */
    property color color: square
                            ? theme.palette.normal.background
                            : theme.palette.normal.overlay

    property point target
    property string direction: "down"
    property bool clipContent: false
    default property alias children: content.children

    implicitWidth: units.gu(10)
    implicitHeight: units.gu(8)

    signal showCompleted()
    signal hideCompleted()

    opacity: 0.0

    function show() {
        hideAnimation.stop();
        showAnimation.start();
    }

    function hide() {
        showAnimation.stop();
        hideAnimation.start();
    }

    ParallelAnimation {
        id: showAnimation

        NumberAnimation {
            target: bubbleShape
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: UbuntuAnimation.FastDuration
            easing: UbuntuAnimation.StandardEasing
        }
        NumberAnimation {
            target: scaleTransform
            property: (direction === "up" || direction === "down") ? "yScale" : "xScale"
            from: 0.91
            to: 1.0
            duration: UbuntuAnimation.FastDuration
            easing: UbuntuAnimation.StandardEasing
        }
        onStopped: showCompleted()
    }

    NumberAnimation {
        id: hideAnimation
        target: bubbleShape
        property: "opacity"
        from: 1.0
        to: 0.0
        duration: UbuntuAnimation.FastDuration
        easing: UbuntuAnimation.StandardEasing
        onStopped: hideCompleted()
    }

    transform: Scale {
        id: scaleTransform
        origin.x: direction === "right" ? bubbleShape.width :
                  direction === "left" ? 0 :
                                          bubbleShape.width/2.0
        origin.y: direction === "up" ? 0 :
                  direction === "down" ? bubbleShape.height :
                                         bubbleShape.height/2.0
    }

    BorderImage {
        id: shadow
        anchors.fill: parent
        anchors.margins: square ? -units.gu(1) : -units.dp(2)
        anchors.topMargin: square ? 0 : anchors.margins
        source: !square ? Qt.resolvedUrl("../artwork/bubble_shadow.sci") : Qt.resolvedUrl("../artwork/header_overflow_dropshadow.sci")
        opacity: 0.8
    }

    UbuntuShape {
        anchors.fill: parent
        aspect: UbuntuShape.Flat
        backgroundColor: bubbleShape.color
        source: bubbleShape.clipContent ? shapeSource : null
        visible: !square
    }

    ShaderEffectSource {
        id: shapeSource
        visible: bubbleShape.clipContent
        sourceItem: bubbleShape.clipContent ? content : null
        hideSource: !square
        // FIXME: visible: false prevents rendering so make it a nearly
        // transparent 1x1 pixel instead
        opacity: 0.01
        width: 1
        height: 1
    }

    Item {
        id: content
        anchors.fill: parent

        Rectangle {
            id: colorRect
            anchors.fill: parent
            color: bubbleShape.color
            visible: bubbleShape.clipContent
        }
    }
}
