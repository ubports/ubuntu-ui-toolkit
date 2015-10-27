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

Page {
    id: page
    TemplateSection {
        title: "Bottom Edge Hint"
        className: "BottomEdgeHint"

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
    }

    Component {
        id: bottomEdgeContent
        Rectangle {
            width: units.gu(40)
            height: page.height
            color: UbuntuColors.green
            Label {
                text: {
                    switch (bottomEdge.status) {
                    case BottomEdge.Fading: return "Status: Fading"
                    case BottomEdge.Idle: return "Status: Idle"
                    case BottomEdge.Hinting: return "Status: Hinting"
                    case BottomEdge.Revealed: return "Status: Revealed"
                    case BottomEdge.CanCommit: return "Status: CanCommit"
                    case BottomEdge.Committed: return "Status: Committed"
                    default: return ""
                    }
                }
                color: "white"
                font.bold: true
                textSize: Label.Large
                anchors.horizontalCenter: parent.horizontalCenter
            }
            MouseArea {
                anchors.fill: parent
            }
        }
    }

    BottomEdge {
        id: bottomEdge
        visible: active
        hint: BottomEdgeHint {
            text: "Compose a new message"
            iconName: "stock_message"
        }
        height: page.height - units.gu(6)
        contentComponent: bottomEdgeContent
        onCommitStarted: print("START COMMIT")
        onCommitCompleted: print("END COMMIT")
        onCollapseStarted: print("START COLLAPSE")
        onCollapseCompleted: print("END COLLAPSE")
    }
}
