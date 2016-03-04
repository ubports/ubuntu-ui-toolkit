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
import Ubuntu.Components.Styles 1.3

BottomEdgeStyle {
    id: bottomEdgeStyle
    //setup properties
    property BottomEdge bottomEdge: styledItem
    panel: panelItem
    panelAnimation: panelBehavior
    revealThreshold: bottomEdge.hint.height + units.gu(2)

    //deprecated for now
    contentItem: bottomEdge.contentItem

    // own styling properties
    property color backgroundColor: !panelItem.collapsing
                                    ? Qt.rgba(0, 0, 0, Math.min(0.25, (height - revealThreshold - panelItem.y) / (height - revealThreshold)))
                                    : "transparent"
    property color panelColor: theme.palette.normal.overlay
    property color shadowColor: "#000000"

    anchors.fill: parent

    Rectangle {
        id: background
        anchors.fill: parent
        color: backgroundColor
        z: -1
    }

    Rectangle {
        id: panelItem
        objectName: "bottomedge_panel"
        property bool collapsing: false
        anchors {
            left: parent.left
            right: parent.right
            top: parent.bottom
            topMargin: bottomEdge.status >= BottomEdge.Revealed
                       ? -(bottomEdge.height * bottomEdge.dragProgress)
                       : 0
        }
        height: bottomEdge.contentItem ? bottomEdge.contentItem.height : 0
        color: panelColor
        opacity: bottomEdge.status >= BottomEdge.Revealed ? 1.0 : 0.0

        Behavior on anchors.topMargin { UbuntuNumberAnimation { id: panelBehavior } }

        Connections {
            target: bottomEdge
            onCollapseStarted: panelItem.collapsing = true
            onCollapseCompleted: panelItem.collapsing = false
        }

        state: bottomEdge.status > BottomEdge.Hidden ? "drop-hint" : ""
        states: [
            State {
                name: "drop-hint"
                PropertyChanges {
                    target: bottomEdge.hint
                    restoreEntryValues: false
                    status: BottomEdgeHint.Inactive
                }
            }
        ]

        // shadows
        Rectangle {
            id: topShadow
            anchors {
                bottom: parent.top
                left: parent.left
                right: parent.right
            }
            height: units.gu(1.5)
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.0) }
                GradientStop { position: 0.75; color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.05) }
                GradientStop { position: 1.0; color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.1) }
            }
        }
        Rectangle {
            id: bottomShadow
            anchors {
                top: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: units.gu(1.5)
            rotation: 180
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.0) }
                GradientStop { position: 0.75; color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.05) }
                GradientStop { position: 1.0; color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.1) }
            }
        }
    }
}
