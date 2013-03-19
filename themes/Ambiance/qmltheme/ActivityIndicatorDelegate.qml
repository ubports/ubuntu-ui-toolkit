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
    property url source
    property PropertyAnimation animation

    anchors.fill: parent
    visible: item.running

    Image {
        id: container
        anchors.fill: parent
        source: visuals.source
        smooth: true
        fillMode: Image.PreserveAspectFit
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter

        states: State {
            name: 'animate'
            when: item.running && (animation != null)
            PropertyChanges {
                target: container
                rotation: 360
            }
        }
        transitions: [
            Transition {
                from: ""
                to: "animate"
                RotationAnimation {
                    loops: Animation.Infinite
                    duration: visuals.animation.duration
                }
            }
        ]
    }
}
