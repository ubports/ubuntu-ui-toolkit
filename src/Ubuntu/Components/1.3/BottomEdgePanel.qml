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
    id: background
    // properties BottomEdge expects
    property alias panelItem: panel
    property alias contentLoader: loader
    property PropertyAnimation panelAnimation: UbuntuNumberAnimation { duration: 1000 }

    anchors.bottom: parent.bottom
    width: bottomEdge.width
    height: bottomEdge.height
    x: updatePosition()
    z: Number.MAX_VALUE
    color: Qt.rgba(1, 1, 1, bottomEdge.dragProgress)

    function updatePosition() {
        var x = background.mapFromItem(bottomEdge, bottomEdge.x, bottomEdge.y).x;
        print(bottomEdge.x, x);
        return x;
    }

    Binding {
        target: bottomEdge.hint
        when: bottomEdge.hint && bottomEdge.state > BottomEdge.Hidden
        property: "state"
        value: "Locked"
    }

    Rectangle {
        id: panel
        objectName: "bottomedge_panel"
        anchors {
            left: parent.left
            right: parent.right
        }
        height: bottomEdge.height
        y: bottomEdge.height

        Behavior on y {
            animation: panelAnimation
        }

        Loader {
            id: loader
            anchors.horizontalCenter: parent.horizontalCenter
            asynchronous: true
            source: bottomEdge.content
            sourceComponent: bottomEdge.contentComponent
            onItemChanged: {
                if (item) {
                    item.parent = panel;
                    item.anchors.horizontalCenter = panel.horizontalCenter;
                }
            }
        }
    }

    // FIXME: use SwipeArea when ready, however keep this as it has to work with mouse drag as well
    MouseArea {
        id: hintArea
        parent: bottomEdge.hint
        anchors.fill: parent
        enabled: bottomEdge.hint && (bottomEdge.hint.state == "Active" || bottomEdge.hint.state == "Locked")

        drag {
            axis: Drag.YAxis
            target: panel
            minimumY: 0
            maximumY: bottomEdge.height
        }

        onReleased: {
            if (bottomEdge.currentSection) {
                bottomEdge.currentSection.dragEnded();
            } else {
                bottomEdge.collapse();
            }
        }
    }

}
