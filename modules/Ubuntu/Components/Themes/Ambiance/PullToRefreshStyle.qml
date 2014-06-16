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

Style.PullToRefreshStyle {
    id: style
    implicitHeight: Math.max(label.paintedHeight, refreshIndicatorItem.height) + units.gu(5)

    // additional configuration properties provided by the Ambiance theme
    // these properties can be used by the deriving themes to configure the label
    // and the activity indicator
    property Item label: control.contentItem
    property alias refreshIndicator: refreshIndicatorItem

    // local properties
    readonly property PullToRefresh control: styledItem
    // property to store Flickable's toipMargin at the time the pull or auto-refresh is started
    property real flickableTopMargin: 0.0
    // store when the drag has happened at the beginning of the Flickable's content
    property bool wasAtYBeginning: false
    // initial contentY value when pull started
    property real initialContentY: 0.0
    // indicates that the refresh has been started manually
//    manualRefresh: false
    // drives the refreshing state
    property bool refreshing: false
    // point of release used in rebind animation between the ready-to-refresh and refreshing states
    property real pointOfRelease
    // specifies the component completion
    property bool ready: false

    anchors.fill: parent

    Component.onCompleted: ready = true;

    onLabelChanged: {
        if (label) {
            label.parent = style;
            label.anchors.fill = style;
        }
    }

    // visuals
    ActivityIndicator {
        id: refreshIndicatorItem
        running: false
        anchors.centerIn: parent
    }

    // state and content controlling
    Connections {
        target: control
        onRefreshingChanged: {
            if (!ready || !control.enabled) {
                return;
            }
            if (!style.manualRefresh && target.refreshing) {
                // not a manual refresh, update flickable's starting topMargin
                style.flickableTopMargin = control.target.topMargin;
                style.wasAtYBeginning = control.target.atYBeginning;
            }
            /*
              We cannot bind refreshing state activation clause with the
              control.refreshing property dirrectly as when the model is
              refreshed automatically (not manually via the component), we
              need to remember the flickable's topMargin in order to proceed
              with a proper Flickable rebinding. If we use property binding,
              the Connections' onRefreshingChanged will update the flickableTopMargin
              only after the binding is evaluated.
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

//    onStateChanged: print("state=", control.objectName + "/" + state)
    states: [
        State {
            name: "disabled"
            when: !control.enabled
            PropertyChanges {
                target: label
                visible: false
            }
        },
        State {
            name: "idle"
            extend: ""
            when: ready && control.enabled && !style.refreshing && !style.manualRefresh
        },
        State {
            name: "ready-to-refresh"
            when: ready && control.enabled && style.manualRefresh && !style.refreshing
        },
        State {
            name: "refreshing"
            when: ready && control.enabled && style.wasAtYBeginning && style.refreshing
            PropertyChanges {
                target: label
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
                    script: control.refresh()
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
            UbuntuNumberAnimation {
                target: control.target
                property: "topMargin"
            }
        }
    ]
}

