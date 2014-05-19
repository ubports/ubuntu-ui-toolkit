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
    id: style
    implicitHeight: Math.max(labelItem.paintedHeight, refreshIndicatorItem.height) + units.gu(5)

    // additional configuration properties provided by the Ambiance theme
    // these properties can be used by the deriving themes to configure the label
    // and the activity indicator
    property alias label: labelItem
    property alias refreshIndicator: refreshIndicatorItem

    // local properties
    readonly property RefreshControl control: styledItem
    property real flickableTopMargin: 0.0
    property bool wasAtYBeginning: false
    property real initialContentY: 0.0
    property bool manualRefresh: false
    property bool refreshing: false
    property real pointOfRelease

    anchors.fill: parent

    // visuals
    Label {
        id: labelItem
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: styledItem.pullText
    }
    ActivityIndicator {
        id: refreshIndicatorItem
        running: false
        anchors.centerIn: parent
    }

    // state controlling
    Connections {
        target: control
        onRefreshingChanged: {
            if (!control.ready || !control.enabled) {
                return;
            }
            if (!style.manualRefresh && target.refreshing) {
                // not a manual refresh, update flickable's starting topMargin
                style.flickableTopMargin = control.target.topMargin;
            }
            /*
              Must controll refreshing property separately and not via property binding as
              when the model is refreshed automatically and not via the component, we need
              to remember the flickable's topMargin in order to proceed with a proper flickable
              rebinding. If we use property binding, the Connections' onRefreshingChanged will
              update the flickableTopMargin only after the binding is evaluated.
              */
            style.refreshing = target.refreshing;
        }
    }
    Connections {
        target: control.target
        onMovementStarted: {
            style.wasAtYBeginning = control.target.atYBeginning;
            style.initialContentY = control.target.contentY;
            style.refreshing = false;
            style.manualRefresh = false;
        }
        onMovementEnded: style.wasAtYBeginning = control.target.atYBeginning

        // catch when to initiate refresh
        onDraggingChanged: {
            if (!control.parent.dragging && style.manualRefresh) {
                pointOfRelease = -(control.target.contentY - control.target.originY)
                style.flickableTopMargin = control.target.topMargin;
                style.refreshing = true;
                style.manualRefresh = false;
            }
        }
        onContentYChanged: {
            if (style.wasAtYBeginning && control.enabled && control.target.dragging) {
                style.manualRefresh = ((style.initialContentY - control.target.contentY) > style.activationThreshold);
            }
        }
    }

    onStateChanged: print("state="+state)
    states: [
        State {
            name: "disabled"
            when: !control.enabled
            PropertyChanges {
                target: labelItem
                visible: false
            }
        },
        State {
            name: "idle"
            extend: ""
            when: control.ready && control.enabled && !style.refreshing && !style.manualRefresh
        },
        State {
            name: "ready-to-refresh"
            when: control.ready && control.enabled && style.manualRefresh && !style.refreshing
        },
        State {
            name: "refreshing"
            when: control.ready && control.enabled && style.wasAtYBeginning && style.refreshing
            PropertyChanges {
                target: labelItem
                visible: false
            }
            PropertyChanges {
                target: refreshIndicatorItem
                running: true
            }
            PropertyChanges {
                target: control.target
                topMargin: style.flickableTopMargin + control.height
            }
        }
    ]

    transitions: [
        // text animations
        Transition {
            from: "idle"
            to: "ready-to-refresh"
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: labelItem
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                }
                ScriptAction {
                    script: labelItem.text = control.releaseText;
                }
                UbuntuNumberAnimation {
                    target: labelItem
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
                    target: labelItem
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                }
                ScriptAction {
                    script: labelItem.text = control.pullText;
                }
                UbuntuNumberAnimation {
                    target: labelItem
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
                    script: style.refresh()
                }
            }
        },
        // transition to be applied when the model is auto-updating
        Transition {
            from: "idle"
            to: "refreshing"
            UbuntuNumberAnimation {
                target: control.target
                property: "contentY"
                from: -style.flickableTopMargin
                to: -style.flickableTopMargin - control.height
            }
        },
        Transition {
            from: "refreshing"
            to: "idle"
            SequentialAnimation {
                ScriptAction {
                    script: labelItem.text = "";
                }
                UbuntuNumberAnimation {
                    target: control.target
                    property: "topMargin"
                }
                ScriptAction {
                    script: labelItem.text = control.pullText;
                }
            }
        }
    ]
}

