/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1
import Ubuntu.Components.Styles 1.1 as Style

Style.RefreshControlStyle {
    property real flickableTopMargin: 0.0
    property real layoutHeight: Math.max(pullLabel.paintedHeight, busyIndicator.height) + units.gu(2)
    activationThreshold: control.height + units.gu(1) + flickableTopMargin

    // local properties
    readonly property RefreshControl control: styledItem

    id: style
    implicitHeight: layoutHeight
    anchors {
        left: parent.left
        right: parent.right
    }

    // visuals
    Label {
        id: pullLabel
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: styledItem.pullText
    }
    ActivityIndicator {
        id: busyIndicator
        running: false
        anchors.centerIn: parent
    }

    // capture topMargin change of the flickable
    Connections {
        target: control.target
        onTopMarginChanged: {
            if (state === "") {
                flickableTopMargin = control.target.topMargin;
            }
        }
    }

    onStateChanged: print("state="+state)
    states: [
        State {
            name: "ready-to-refresh"
        },
        State {
            name: "refreshing"
            PropertyChanges {
                target: pullLabel
                visible: false
            }
            PropertyChanges {
                target: busyIndicator
                running: true
            }
            PropertyChanges {
                target: control.target
                topMargin: style.flickableTopMargin + layoutHeight
            }
        }
    ]
    function flipLabel() {
        if (pullLabel.text === control.pullText) {
            pullLabel.text = control.releaseText;
        } else {
            pullLabel.text = control.pullText;
        }
    }

    transitions: [
        Transition {
            from: ""
            to: "ready-to-refresh"
            SequentialAnimation {
                NumberAnimation { target: pullLabel; property: "opacity"; from: 1.0; to: 0.0; duration: UbuntuAnimation.FastDuration; easing: UbuntuAnimation.StandardEasing }
                ScriptAction { script: flipLabel() }
                NumberAnimation { target: pullLabel; property: "opacity"; from: 0.0; to: 1.0; duration: UbuntuAnimation.FastDuration; easing: UbuntuAnimation.StandardEasing }
            }
        },
        Transition {
            from: "ready-to-refresh"
            to: ""
            SequentialAnimation {
                NumberAnimation { target: pullLabel; property: "opacity"; from: 1.0; to: 0.0; duration: UbuntuAnimation.FastDuration; easing: UbuntuAnimation.StandardEasing }
                ScriptAction { script: flipLabel() }
                NumberAnimation { target: pullLabel; property: "opacity"; from: 0.0; to: 1.0; duration: UbuntuAnimation.FastDuration; easing: UbuntuAnimation.StandardEasing }
            }
        },
        Transition {
            from: "refreshing"
            to: ""
            PropertyAnimation {
                target: control.target
                property: "topMargin"
                from: flickableTopMargin + layoutHeight
                to: flickableTopMargin
                duration: UbuntuAnimation.FastDuration
                easing: UbuntuAnimation.StandardEasing
            }
        }
    ]
}

