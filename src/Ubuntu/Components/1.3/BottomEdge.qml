/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: bottomEdge

    // hint API
    property string iconName
    property url iconSource
    property string text

    // progress
    property real progress: 0.0
    // TODO: move this as enum
    property string status: "Hinted"

    // content
    property Component contentComponent
    property url contentSource
    property Component fakeContent
    readonly property Item contentItem: bottomEdgeLoader.item

    signal openStarted
    signal openEnded
    signal clicked

    property Item flickable

    function open() {
        bottomEdge.state = "expanded";
    }

    function close() {
        bottomEdge.state = "collapsed";
    }

    visible: enabled
    anchors.fill: parent
    Item {
        id: bottomEdgeBody
        anchors {
            left: parent.left
            right: parent.right
        }
        height: bottomEdgeContent.height

        Item {
            id: bottomEdgeContent
            anchors {
                left: parent.left
                right: parent.right
            }
            height: bottomEdgeLoader.height

            Item {
                id: bottomEdgeShadows
                anchors.fill: bottomEdgeContent

                BottomEdgeShadow {
                    anchors.bottom: parent.top
                }

                BottomEdgeShadow {
                    anchors.top: parent.bottom
                    rotation: 180
                }
            }

            Rectangle {
                anchors.fill: parent
                color: Theme.palette.normal.background
            }

            Loader {
                id: bottomEdgeLoader
                source: bottomEdge.contentSource
                sourceComponent: bottomEdge.contentComponent
                asynchronous: true
            }
        }


        BottomEdgeHint {
            id: bottomEdgeHint
            anchors.bottom: bottomEdgeBody.top
            iconName: bottomEdge.iconName
            onClicked: bottomEdge.clicked()

            Connections {
                target: bottomEdgeDragArea
                onClosedChanged: {
                    if (!bottomEdgeDragArea.closed) {
                        bottomEdgeHint.state = "Visible";
                    }
                }
            }

            Connections {
                target: flickable
                onVerticalVelocityChanged: {
                    if (!bottomEdgeDragArea.closed) {
                        return;
                    }

                    if (flickable.verticalVelocity > 0) {
                        bottomEdgeHint.state = "Hidden";
                    } else if (flickable.verticalVelocity < 0) {
                        bottomEdgeHint.state = "Visible";
                    }
                }
            }
        }
    }


    state: "collapsed"
    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: bottomEdgeBody
                y: bottomEdgeDragArea.drag.maximumY
            }
            PropertyChanges {
                target: bottomEdgeContent
                visible: false
            }
        },
        State {
            name: "expanded"
            ParentChange {
                target: bottomEdgeContent
                parent: bottomEdge
                x: 0
                y: 0
            }
            PropertyChanges {
                target: bottomEdgeBody
                y: bottomEdgeDragArea.drag.maximumY
                opacity: 0.0
            }
            PropertyChanges {
                target: bottomEdgeShadows
                opacity: 0.0
                visible: true
            }
        },
        State {
            name: "floating"
            when: bottomEdgeDragArea.drag.active
            PropertyChanges {
                target: bottomEdgeContent
                visible: true
            }
        }
    ]

    transitions: [
        Transition {
            to: "collapsed"
            SequentialAnimation {
                alwaysRunToEnd: true

                SmoothedAnimation {
                    target: bottomEdgeBody
                    property: "y"
                    duration: UbuntuAnimation.SlowDuration
                }
                ScriptAction {
                    script: {
                        bottomEdgeLoader.active = false;
                        bottomEdgeLoader.active = true;
                    }
                }
            }
        },
        Transition {
            to: "expanded"
            SequentialAnimation {
                ParallelAnimation {
                    ScriptAction {
                        script: bottomEdge.openStarted()
                    }
                    ParentAnimation {
                        UbuntuNumberAnimation {
                            properties: "x,y"
                            duration: UbuntuAnimation.SlowDuration
                            target: bottomEdgeContent
                        }
                    }
                    UbuntuNumberAnimation {
                        target: bottomEdgeShadows
                        property: "opacity"
                        duration: UbuntuAnimation.SlowDuration
                    }
                }
                UbuntuNumberAnimation {
                    target: bottomEdgeContent
                    property: "opacity"
                    duration: UbuntuAnimation.FastDuration
                }
                ScriptAction {
                    script: bottomEdge.openEnded()
                }
            }
        }
    ]

    MouseArea {
        id: bottomEdgeDragArea

        property real previousY: -1
        property string dragDirection: "None"
        property bool closed: drag.target.y == bottomEdgeDragArea.drag.maximumY
                            && !bottomEdgeDragArea.pressed

        preventStealing: true
        propagateComposedEvents: true
        drag {
            axis: Drag.YAxis
            target: bottomEdgeBody
            minimumY: 0
            maximumY: bottomEdge.height
        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: bottomEdgeHint.height

        onPressed: {
            previousY = mouse.y;
        }

        onReleased: {
            if (dragDirection === "BottomToTop") {
                bottomEdge.state = "expanded";
            } else {
                bottomEdge.state = "collapsed";
            }
            previousY = -1;
            dragDirection = "None";
        }

        onMouseYChanged: {
            var yOffset = previousY - mouseY;
            // skip if was a small move
            if (Math.abs(yOffset) <= units.gu(2)) {
                return;
            }
            previousY = mouseY;
            dragDirection = yOffset > 0 ? "BottomToTop" : "TopToBottom";
        }
    }
}
