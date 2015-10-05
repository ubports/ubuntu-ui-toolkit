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

Rectangle {
    id: panel
    anchors.fill: parent
    color: "transparent"

//    Connections {
//        target: styledItem
//        onContentChanged: {
//        }
//    }

    Component {
        id: hintComponent
        Item {
            id: hint
            property BottomEdgeProxy bottomEdge: modelData
            property bool hintedAbove: false
            height: units.gu(4)
            width: bottomEdge ? bottomEdge.width : 0
            x: {
                var scenePos = hints.mapFromItem(bottomEdge, bottomEdge.x, bottomEdge.y);
                return scenePos.x + (bottomEdge.width / 2 - width / 2)
            }
            MouseArea {
                anchors {
                    fill: parent
                    leftMargin: units.gu(2)
                    rightMargin: units.gu(2)
                }
                hoverEnabled: true
                onEntered: {
                    backToIdle.stop();
                    hint.state = "hinted";
                    hintedAbove = true;
                }
                onExited: {
                    if (hint.state == "hinted") {
                        backToIdle.start();
                    }
                }
                onClicked: {
                    backToIdle.stop();
                    hint.state = "committed";
                }
            }
            Timer {
                id: backToIdle
                interval: 800
                repeat: false
                onTriggered: hint.state = "idle"
            }

            state: "idle"
            states: [
                State {
                    name: "idle"
                    extend: ""
                    StateChangeScript {
                        script: {
                            bottomEdge.status = BottomEdgeProxy.Idle;
                            hint.hintedAbove = false;
                        }
                    }
                },
                State {
                    name: "hinted"
                    AnchorChanges {
                        target: h1
                        anchors.top: parent.bottom
                        anchors.bottom: undefined
                    }
                    AnchorChanges {
                        target: h2
                        anchors.top: parent.top
                    }
                }
            ]
            transitions: [
                Transition {
                    from: "*"
                    to: "hinted"
                    reversible: true
                    AnchorAnimation {
                        targets: [h1, h2]
                        duration: UbuntuAnimation.BriskDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                }
            ]

            // content
            Icon {
                id: h1
                name: "up"
                width: units.gu(2)
                height: width
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                }
            }
            Rectangle {
                id: h2
                // FIXME: use some themed color here once we get the design
                color: Qt.lighter(UbuntuColors.lightGrey, 1.6)
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.bottom
                }
                height: parent.height
                Row {
                   anchors.centerIn: parent
                   spacing: units.dp(4)
                   Icon {
                       id: icon
                       width: units.gu(2)
                       height: width
                       source: (bottomEdge.iconSource != "") ? bottomEdge.iconSource : "image://theme/" + bottomEdge.iconName
                   }
                   Label {
                       text: bottomEdge.text
                       anchors.verticalCenter: icon.verticalCenter
                   }
                }
            }
        }
    }

    Item {
        id: hints
        anchors {
            left: parent.left
            right: parent.right
            bottom: content.top
        }
        height: childrenRect.height

        Repeater {
            model: styledItem.content
            delegate: hintComponent
        }
    }
    Item {
        id: content
        anchors {
            fill: parent
            topMargin: parent.height
        }
//        states: [
//            State {
//                name: ""
//            }

//        ]
    }

}
