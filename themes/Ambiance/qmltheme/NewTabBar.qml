/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    id: tabBar
    color: active ? "red" : "transparent"

    height: units.gu(6)
    property ListModel tabModel

    property bool active: false
    //    onActiveChanged: buttonView.position()

    property real totalButtonWidth: 0
    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            Component.onCompleted: {
                tabBar.totalButtonWidth = theRow.width;
                print("total buttonw width = "+tabBar.totalButtonWidth);
            }

            Repeater {
                id: repeater
                model: tabModel

                AbstractButton {
                    id: button
                    width: text.width + 2*text.anchors.margins
                    property bool selected: (index === tabs.selectedTabIndex)

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    Rectangle {
                        border.width: 2
                        radius: 10
                        color: selected ? "pink" : "black"
                        anchors.fill: parent
                        visible: true
                    }

                    TextCustom {
                        id: text
                        visible: true // tabBar.active || selected
                        anchors.centerIn: parent
                        anchors.margins: units.gu(2)
                        text: title + tabIndex
                        fontSize: "x-large"
                    }

                    onClicked: {
                        tabs.selectedTabIndex = index;
                        tabBar.active = false;
                    }
                }
            }
        }
    }

    PathView {
        id: buttonView
        anchors.fill: parent
        model: 2
        delegate: tabButtonRow
        highlightRangeMode: PathView.NoHighlightRange
        offset: (model === 1) ? 0.5 : 0
        path: Path {
            startX: -tabBar.totalButtonWidth/2
            PathLine {
                x: tabBar.totalButtonWidth*1.5
            }
        }
        onOffsetChanged: print("offset = "+offset)
    }

    MouseArea {
        // an inactive tabBar can be clicked to make it active
        anchors.fill: parent
        enabled: !tabBar.active
        onClicked: {
            print("activating tab bar!");
            tabBar.active = true;
        }
    }
}
