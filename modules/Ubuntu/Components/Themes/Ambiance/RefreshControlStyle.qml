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
    property real baseFlickableTopMargin: 0.0
    property real layoutHeight: Math.max(pullImage.paintedHeight, pullLabel.paintedHeight, busyIndicator.height) + units.gu(1.5)
    property real flipThreshold: layoutHeight + units.gu(1) + baseFlickableTopMargin
    property real spacing: pullImage.width / 2

    property Flickable flickable: styledItem.target

    id: style
    anchors {
        left: parent.left
        leftMargin: spacing
        right: parent.right
        rightMargin: spacing
    }

    Row {
        id: indication
        spacing: style.spacing
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
            text: styledItem.pullText
        }
    }
    ActivityIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
    }

    onStateChanged: print("state="+state)
    states: [
        State {
            name: ""
            PropertyChanges {
                target: flickable
                topMargin: baseFlickableTopMargin
            }
        },

        State {
            name: "release-to-refresh"
            PropertyChanges {
                target: pullImage
                rotation: -180
            }
            PropertyChanges {
                target: pullLabel
                text: styledItem.releaseText
            }
        },
        State {
            name: "refresh-in-progress"
            PropertyChanges {
                target: indication
                visible: false
            }

            PropertyChanges {
                target: busyIndicator
                running: true
            }
            PropertyChanges {
                target: flickable
                topMargin: style.baseFlickableTopMargin + style.layoutHeight
            }
            AnchorChanges {
                target: style
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.left: undefined
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "release-to-refresh"
            reversible: true
            PropertyAnimation {
                target: pullImage
                property: "rotation"
                duration: UbuntuAnimation.FastDuration
                easing: UbuntuAnimation.StandardEasing
            }
        },
        Transition {
            from: "refresh-in-progress"
            to: "*"
            PropertyAnimation {
                target: flickable
                property: "topMargin"
                duration: UbuntuAnimation.FastDuration
                easing: UbuntuAnimation.StandardEasing
            }
        }
    ]
}

