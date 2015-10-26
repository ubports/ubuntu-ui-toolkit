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
    // properties BottomEdge expects
    property alias panelItem: panel
    property alias contentLoader: loader

    id: background
    anchors.fill: parent
    color: Qt.rgba(0, 0, 0, bottomEdge.dragProgress)
    z: Number.MAX_VALUE

    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        textSize: Label.XLarge
        font.bold: true
        text: "state=" + state
    }

    state: ""
    states: [
        State {
            name: "Committing"
            PropertyChanges {
                target: panel
                restoreEntryValues: false
                y: 0
            }
        },
        State {
            name: "Collapsing"
            PropertyChanges {
                target: panel
                restoreEntryValues: false
                y: bottomEdge.height
            }
        }
    ]
    transitions: Transition {
        from: ""
        to: "*"
        reversible: true
        SequentialAnimation {
            UbuntuNumberAnimation {
                target: panel
                property: "y"
            }
            ScriptAction {
                script: {
                    if (background.state == "Committing") {
                        bottomEdge.commitFinished();
                    } else if (background.state == "Collapsing") {
                        bottomEdge.collapseFinished();
                    }
                    background.state = "";
                }
            }
        }
    }

    Rectangle {
        id: panel
        anchors {
            left: parent.left
            right: parent.right
        }
        height: bottomEdge.height
        y: bottomEdge.height

        Loader {
            id: loader
            anchors.horizontalCenter: parent.horizontalCenter
            asynchronous: true
            source: bottomEdge.content
            sourceComponent: bottomEdge.contentComponent
        }
    }

    // FIXME: use SwipeArea when ready
    MouseArea {
        id: hintArea
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        y: bottomEdge.hint.y
        height: bottomEdge.hint.height

        enabled: bottomEdge.status >= BottomEdge.Active
        drag {
            axis: Drag.YAxis
            target: panel
            minimumY: 0
            maximumY: bottomEdge.height
        }
        onReleased: {
            switch (bottomEdge.status) {
            case BottomEdge.CanCommit:
                bottomEdge.commit();
                break;
            case BottomEdge.Revealed:
                bottomEdge.collapse();
                break;
            }
        }
    }

}
