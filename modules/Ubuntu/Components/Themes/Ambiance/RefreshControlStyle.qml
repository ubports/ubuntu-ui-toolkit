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
//import Ubuntu.Components 0.1

Row {
    id: labelRow
    anchors.left: parent.left
    anchors.leftMargin: spacing
    spacing: pullImage.width / 2
    width: pullImage.width + pullLabel.width + spacing

    Image {
        id: pullImage
        smooth: true
        source: Qt.resolvedUrl("artwork/chevron_down.png")
        rotation: 2 * 360 * pull.contentY / pull.flickable.height
        onRotationChanged: {
            if (pullImage.rotation < -control.rotationThreshold){
                if (!pullTimer.running && !pull.__puller)
                    pullTimer.restart()
            }
            else if (pullImage.rotation > -control.rotationThreshold){
                if (!pullTimer.running && pull.__puller)
                    pullTimer.restart()
            }
        }

        Timer{
            id: pullTimer
            interval: control.latency

            onTriggered: {
                print("trigered")
                if(pullImage.rotation < -control.rotationThreshold)
                    pull.__puller = true
                else
                    pull.__puller = false
            }
        }
    }

    Label {
        id: pullLabel
        text: (pull.__puller) ? control.releaseMessageString : control.pullMessageString
    }
}
