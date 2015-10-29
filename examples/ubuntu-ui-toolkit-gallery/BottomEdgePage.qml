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
    header: PageHeader {
        title: "Bottom Edge"
    }

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

    ListView {
        anchors.fill: parent
        model: 50
        delegate: ListItemLayout {
            title.text: "Item #" + index
        }
    }

    Component {
        id: bottomEdgeContent
        Page {
            onParentChanged: print(parent)
            width: units.gu(40)
            height: parent.height
            // FIXME: this is crap!!!
            anchors.left: undefined
            anchors.bottom: undefined
            header: PageHeader {
                title: {
                    var prefix = "Yohoooo! ";
                    switch (bottomEdge.state) {
                    case BottomEdge.Hidden: return prefix + "Status: Hidden"
                    case BottomEdge.Revealed: return prefix + "Status: Revealed"
                    case BottomEdge.CanCommit: return prefix + "Status: CanCommit"
                    case BottomEdge.Committed: return prefix + "Status: Committed"
                    default: return prefix + "UNKNOWN";
                    }
                }
            }
            Rectangle {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                color: Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1)
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
        height: page.height// - units.gu(6)
        contentComponent: bottomEdgeContent
        onCommitStarted: print("START COMMIT")
        onCommitCompleted: print("END COMMIT")
        onCollapseStarted: print("START COLLAPSE")
        onCollapseCompleted: print("END COLLAPSE")
    }
}
