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
    property real layoutY: -(contentY + pullImage.height + units.gu(1.5))
    property real threshold: styledItem.height + units.gu(1.5)

    function start() {
        state = "pending-refresh";
        flickable.contentY = -units.gu(10)// + flickable.originY;
    }

    function stop() {
        flickable.contentY = 0;
        puller = false;
        print("flickable reset")
        state = "";
    }

    property Flickable flickable: styledItem.target
    property int contentY: flickable ? (flickable.contentY - flickable.originY) : 0
    property int mappedY: styledItem.mapToItem(flickable, 0, contentY).y

    id: style
    anchors {
        left: parent.left
        leftMargin: spacing
    }
    spacing: pullImage.width / 2
    width: pullImage.paintedWidth + pullLabel.paintedWidth + spacing
    //    opacity: -pullImage.rotation / style.rotationThreshold

    Image {
        id: pullImage
        smooth: true
        source: Qt.resolvedUrl("artwork/go-bottom.png")
    }
    Label {
        id: pullLabel
        anchors {
            top: pullImage.top
            bottom: pullImage.bottom
        }
        verticalAlignment: Text.AlignVCenter
        text: styledItem.pullMessageString
    }
    ActivityIndicator {
        id: busyIndicator
        running: !visible
        visible: false
    }

    states: [
        State {
            name: "start-refreshing"
            when: contentY < -threshold
            PropertyChanges {
                target: pullImage
                rotation: -180
            }
            PropertyChanges {
                target: style
                puller: true
            }
            PropertyChanges {
                target: pullLabel
                text: styledItem.releaseMessageString
            }
        },
        State {
            name: "pending-refresh"
            PropertyChanges {
                target: pullImage
                visible: false
                width: 0
            }
            PropertyChanges {
                target: pullLabel
                visible: false
                width: 0
            }
            PropertyChanges {
                target: busyIndicator
                visible: true
            }
        }

    ]
    transitions: [
        Transition {
            from: ""
            to: "start-refreshing"
            reversible: true
            UbuntuNumberAnimation {
                target: pullImage
                property: "rotation"
            }
        }
    ]

    // rebound transition
//    Binding {
//        target: flickable
//        property: "rebound"
//        value: Transition {
//            enabled: puller
//            SequentialAnimation {
//                PropertyAnimation {
//                    id: reboundAnimation
//                    duration: UbuntuAnimation.FastDuration
//                    easing: UbuntuAnimation.StandardEasing
//                    property: "y"
//                    to: units.gu(5) - flickable.originY
//                }
//                ScriptAction {
//                    script: {
//                        flickable.contentY = -units.gu(5) + flickable.originY;
//                        style.state = "pending-refresh";
//                    }
//                }
//            }
//        }
//    }
}

