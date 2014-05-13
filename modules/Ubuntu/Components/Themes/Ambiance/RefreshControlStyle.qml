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
    implicitHeight: Math.max(pullLabel.paintedHeight, busyIndicator.height) + units.gu(4)
    activationThreshold: control.height + units.gu(1) + flickableTopMargin

    // local properties
    readonly property RefreshControl control: styledItem
    property real flickableTopMargin: 0.0
    property bool triggerRefresh: false
    property real contentY: target.contentY - target.originY
    property bool activate: contentY < -activationThreshold
    property real pointOfRelease
    property bool refreshing: false
    property string prevState

    id: style
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

    // refresh logic of the visuals
    Connections {
        target: control
        onRefreshingChanged: {
            if (style.triggerRefresh) {
                if (!target.refreshing) {
                    style.refreshing = false;
                }
            } else {
                style.refreshing = target.refreshing;
            }
        }
    }

    Connections {
        target: control.target
        // capture topMargin change of the flickable
        onTopMarginChanged: {
            if (control.ready && !style.refreshing && !control.target.moving) {
//                print("top", control.target.topMargin, "state=", style.state)
                flickableTopMargin = control.target.topMargin;
            }
        }
        // catch when to initiate refresh
        onDraggingChanged: {
            if (!control.parent.dragging && triggerRefresh) {
                pointOfRelease = -style.contentY
                style.refreshing = true;
            }
        }
    }

    onStateChanged: print("state="+state)
    states: [
        State {
            name: "idle"
            extend: ""
            when: control.ready && !style.refreshing && !style.activate
        },
        State {
            name: "ready-to-refresh"
            when: control.ready && control.enabled && style.activate && !style.refreshing
            PropertyChanges {
                target: style
                triggerRefresh: true
            }
        },
        State {
            name: "refreshing"
            when: control.ready && control.enabled && style.refreshing
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
                topMargin: style.flickableTopMargin + control.height
            }
        }
    ]

    transitions: [
        Transition {
            from: "idle"
            to: "ready-to-refresh"
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: pullLabel
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                }
                ScriptAction {
                    script: pullLabel.text = control.releaseText
                }
                UbuntuNumberAnimation {
                    target: pullLabel
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                }
            }
        },
        Transition {
            from: "ready-to-refresh"
            to: "idle"
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: pullLabel
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                }
                ScriptAction {
                    script: pullLabel.text = control.pullText
                }
                UbuntuNumberAnimation {
                    target: pullLabel
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                }
            }
        },
        Transition {
            from: "ready-to-refresh"
            to: "refreshing"
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: control.target
                    property: "topMargin"
                    from: style.pointOfRelease
                    to: style.flickableTopMargin + control.height
                }
                ScriptAction {
                    script: {
                        style.refresh()
                    }
                }
            }
        },

        Transition {
            from: "refreshing"
            to: "idle"
            SequentialAnimation {
                ScriptAction {
                    script: pullLabel.text = "";
                }
                UbuntuNumberAnimation {
                    target: control.target
                    property: "topMargin"
                    from: flickableTopMargin + control.height
                    to: flickableTopMargin
                }
                ScriptAction {
                    script: pullLabel.text = control.pullText;
                }
            }
        }
    ]
}

