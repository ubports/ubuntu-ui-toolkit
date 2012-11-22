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

    property alias iconHeight: icon.height
    property alias iconWidth: icon.width
    property real leftIconMargin: units.gu(0.5)
    property real rightIconMargin: units.gu(0.5)
    width: visible ? iconWidth + leftIconMargin + rightIconMargin : 0

    anchors {
        top: parent ? parent.top : undefined
        left: parent ? parent.left : undefined
        bottom: parent ? parent.bottom : undefined
    }

    ImageWithFallback {
        id: icon
        visible: !iconVisual.hasFrame
        opacity: iconVisual.enabled ? 1.0 : 0.5
        fillMode: Image.PreserveAspectCrop
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: iconVisual.leftIconMargin
        }
        height: sourceSize.height
        width: sourceSize.width
        smooth: true
        asynchronous: true
    }


    UbuntuShape {
        visible: iconVisual.hasFrame
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: iconVisual.leftIconMargin
        }
        height: icon.height
        width: icon.width

        image: icon
    }

    //    BorderImage {
    //        id: iconFrame
    //        opacity: iconVisual.enabled ? 1.0 : 0.5
    //        visible: iconVisual.hasFrame
    //        source: visible ? "artwork/ListItemFrame.png" : ""
    //        anchors.fill: icon
    //        border {
    //            left: units.dp(3)
    //            right: units.dp(3)
    //            top: units.dp(3)
    //            bottom: units.dp(3)
    //        }
    //        horizontalTileMode: BorderImage.Stretch
    //        verticalTileMode: BorderImage.Stretch
    //    }
}
