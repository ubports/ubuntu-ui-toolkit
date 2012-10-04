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

// internal helper class to create the visuals
// for the icon.
Item {
    id: iconVisual

    property alias source: icon.source
    property alias fallbackSource: icon.fallbackSource
    visible: source != ""
    property bool hasFrame: true

    property real leftIconMargin: units.dp(5)
    property real rightIconMargin: units.dp(5)
    property real topIconMargin: units.dp(5)
    property real bottomIconMargin: units.dp(5)
    width: visible ? icon.width + leftIconMargin + rightIconMargin : 0

    anchors {
        top: parent ? parent.top : undefined
        left: parent ? parent.left : undefined
        bottom: parent ? parent.bottom : undefined
    }

    ImageWithFallback {
        id: icon
        opacity: iconVisual.enabled ? 1.0 : 0.5
        fillMode: Image.PreserveAspectFit
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: iconVisual.leftIconMargin
        }
        property real sourceAspectRatio: sourceSize.width / sourceSize.height
        height: Math.min(units.dp(sourceSize.height), iconVisual.height - iconVisual.topIconMargin - iconVisual.bottomIconMargin)
        width: sourceAspectRatio * height
        smooth: true
        asynchronous: true
    }

    BorderImage {
        id: iconFrame
        opacity: iconVisual.enabled ? 1.0 : 0.5
        visible: iconVisual.hasFrame
        source: visible ? "artwork/ListItemFrame.png" : ""
        anchors.fill: icon
        border {
            left: 3
            right: 3
            top: 3
            bottom: 3
        }
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
    }
}
