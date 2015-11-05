/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3

Item {
    id: bottomEdgeHintStyle
    implicitWidth: styledItem.parent.width
    implicitHeight: units.gu(4)

    // initial state
    state: styledItem.status == BottomEdgeHint.Locked ? "Locked" : "Inactive"

    Connections {
        target: styledItem
        onLockedChanged: bottomEdgeHintStyle.state = styledItem.status == BottomEdgeHint.Locked ? "Locked" : "Inactive"
    }

    states: [
        State {
            name: "Inactive"
            extend: ""
            StateChangeScript {
                script: turnToIdleTimer.stop()
            }
        },
        State {
            name: "Active"
            PropertyChanges {
                target: h1
                anchors.verticalCenterOffset: styledItem.height / 2
            }
            PropertyChanges {
                target: h2
                anchors.topMargin: 0
            }
        },
        State {
            name: "Hidden"
            when: styledItem.flickable
                  && (styledItem.flickable.flicking || styledItem.flickable.moving)
                  && (styledItem.status != BottomEdgeHint.Locked)
            PropertyChanges {
                target: styledItem
                opacity: 0.0
            }
        },
        // FIXME: locked should be set and be final if mouse is attached
        // requires QSystemInfo support, which is ongoing work upstream
        State {
            name: "Locked"
            PropertyChanges {
                target: h1
                anchors.verticalCenterOffset: styledItem.height / 2
            }
            PropertyChanges {
                target: h2
                anchors.topMargin: 0
            }
            PropertyChanges {
                target: turnToIdleTimer
                running: false

            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "Active"
            reversible: true
            UbuntuNumberAnimation {
                targets: [h1, h2]
                properties: "anchors.verticalCenterOffset, anchors.topMargin"
            }
        },
        Transition {
            from: "*"
            to: "Hidden"
            reversible: true
            UbuntuNumberAnimation {
                target: styledItem
                property: "opacity"
                duration: UbuntuAnimation.SlowDuration
            }
        }
    ]

    // FIXME: maybe use SwipeGesture once available
    MultiPointTouchArea {
        id: touchDetector
        anchors.fill: parent
        mouseEnabled: false
        minimumTouchPoints: 1
        maximumTouchPoints: 1

        onGestureStarted: {
            var point = gesture.touchPoints[0];
            if (!styledItem.contains(Qt.point(point.x, point.y))) {
                return;
            }
            bottomEdgeHintStyle.state = "Active";
            turnToIdleTimer.stop();
        }
        onReleased: if (bottomEdgeHintStyle.state == "Active") turnToIdleTimer.restart()
    }

    Timer {
        id: turnToIdleTimer
        interval: 800
        repeat: false
        onTriggered: bottomEdgeHintStyle.state = "Inactive"
    }

    clip: true

    Icon {
        id: h1
        width: units.gu(2)
        height: width
        anchors {
            centerIn: parent
            topMargin: styledItem.height
        }
        name: "up"
    }

    Rectangle {
        id: h2
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: styledItem.height
        }
        height: styledItem.height
        color: theme.palette.normal.overlay
        ThinDivider {
            anchors.top: parent.top
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                Haptics.play();
                styledItem.clicked();
                mouse.accepted = false;
            }
        }

        Row {
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            spacing: units.gu(1)
            Icon {
                id: icon
                name: styledItem.iconName
                source: styledItem.iconSource
                width: height
                height: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
                color: theme.palette.normal.overlayText
            }
            Label {
                id: label
                text: styledItem.text
                textSize: Label.Medium
                color: theme.palette.normal.overlayText
                height: styledItem.height
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
