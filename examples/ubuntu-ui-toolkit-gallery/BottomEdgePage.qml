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
        id: listView
        anchors.fill: parent
        model: 50
        delegate: ListItemLayout {
            title.text: "Item #" + index
        }
    }

    Component {
        id: bottomEdgeContent
        Page {
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
                    case BottomEdge.SectionCommitted: return prefix + "Status: SectionCommitted"
                    case BottomEdge.Committed: return prefix + "Status: Committed"
                    default: return prefix + "UNKNOWN";
                    }
                }
            }
            Rectangle {
                anchors.fill: parent
                anchors.margins: units.gu(1)
                color: bottomEdge.activeRange ?
                           bottomEdge.activeRange.baseColor : Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1)
            }
        }
    }

    BottomEdge {
        id: bottomEdge
        visible: active
        hint: BottomEdgeHint {
            text: "Compose a new message"
            iconName: "stock_message"
            flickable: listView
            deactivateTimeout: 1000
        }
        height: page.height// - units.gu(6)
        contentComponent: bottomEdgeContent
        onCommitStarted: print("START COMMIT")
        onCommitCompleted: print("END COMMIT")
        onCollapseStarted: print("START COLLAPSE")
        onCollapseCompleted: print("END COLLAPSE")

        BottomEdgeRange {
            objectName: "FirstSection"
            from: 0.2
            to: 0.4
            property color baseColor: Qt.rgba(0.5, 0.4, bottomEdge.dragProgress, 1)
            contentComponent: Rectangle {
                anchors.fill: parent
                border.width: units.gu(1)
                PageHeader {
                    title: "Not a page"
                }
            }
        }
        BottomEdgeRange {
            objectName: "SecondSection"
            from: 0.4
            to: 0.8
            property color baseColor: Qt.rgba(1, 0.4, bottomEdge.dragProgress, 1)
        }
        BottomEdgeRange {
            objectName: "ThirdSection"
            from: 0.8
            to: bottomEdge.commitPoint
            property color baseColor: Qt.rgba(0, 1, bottomEdge.dragProgress, 1)
        }
    }
}
