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

Row {

    property bool puller: false
    property real rotationThreshold: 135.0
    property int latency: 600
    property real layoutY: -(contentY + pullImage.height + spacing)

    function stop() {
        puller = false;
        pullTimer.stop();
    }

    property Item flickable: styledItem.parent
    property int originY: flickable && flickable.hasOwnProperty("originY") ? flickable.originY : 0
    property int contentY: (flickable ? flickable.contentY : 0) - originY
    property int mappedY: styledItem.mapToItem(flickable, 0, contentY).y

    id: style
    anchors.left: parent.left
    anchors.leftMargin: spacing
    spacing: pullImage.width / 2
    width: pullImage.width + pullLabel.width + spacing
//    opacity: -pullImage.rotation / style.rotationThreshold

    Image {
        id: pullImage
        smooth: true
        source: Qt.resolvedUrl("artwork/go-top.png")
        rotation: 2 * 360 * style.contentY / style.flickable.height
        onRotationChanged: {
            if (pullImage.rotation < -style.rotationThreshold){
                if (!pullTimer.running && !puller)
                    pullTimer.restart()
            }
            else if (pullImage.rotation > -style.rotationThreshold){
                if (!pullTimer.running && puller)
                    pullTimer.restart()
            }
        }

        Timer{
            id: pullTimer
            interval: style.latency
            onTriggered: {
                puller = (pullImage.rotation < -style.rotationThreshold);
            }
        }
    }

    Label {
        id: pullLabel
        text: (puller) ? styledItem.releaseMessageString : styledItem.pullMessageString
    }
}
