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
    //    color: active ? "tan" : "transparent"
    color: "transparent"

    height: units.gu(6)

    property Tabs tabs
    property ListModel tabModel
    //    property int selectedTabIndex
    //    onSelectedTabIndexChanged: buttonView.position()

    Connections {
        target: tabs
        onSelectedTabIndexChanged: {
            tabBar.active = false;
            buttonView.position()
        }
    }

    property bool active: false
    onActiveChanged: {
        print("active = "+active);
        buttonView.position();
    }

    property real totalButtonWidth: 0
    property var relativeButtonPositions

    Component {
        id: tabButtonRow
        Row {
            id: theRow
            anchors {
                top: parent.top
                bottom: parent.bottom
                //                left: parent.left
            }
            width: childrenRect.width

            Component.onCompleted: {
                print("row parent = "+parent)
                print("row width = "+theRow.width)
                tabBar.totalButtonWidth = theRow.width;
                tabBar.relativeButtonPositions = [];
                // children[length-3] is the repeater
                // children[length-2] is the chevron
                for (var i=0; i < children.length; i++) print(children[i])
                for (var i=0; i < children.length-1; i++) {
                    print(children[i].x);
                    tabBar.relativeButtonPositions.push(children[i].x / tabBar.totalButtonWidth);
                }
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

                    //                    Rectangle {
                    //                        border.width: 2
                    //                        radius: 10
                    //                        color: selected ? "pink" : "black"
                    //                        anchors.fill: parent
                    //                        visible: true
                    //                    }

                    TextCustom {
                        id: text
                        color: "#333333"
                        opacity: selected ? 0.9 : 0.6
                        visible: tabBar.active || selected
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

            //            Image {
            //                source: "artwork/chevron.png"
            //            }
        }
    }

//    Item {
        //        color: "black"
        //        id: clipper
        //        anchors {
        //            left: tabBar.left
        //            top: tabBar.top
        //            bottom: tabBar.bottom
        //            right: tabBar.right
        //        }

//        anchors.fill: parent
        //        visible: true
        //        width: Math.min(tabBar.width, tabBar.totalButtonWidth)

        PathView {
            id: buttonView
            //            anchors.fill: parent
            anchors {
                left: parent.left
//                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            clip: true
            interactive: (tabBar.totalButtonWidth > tabBar.width)
            width: Math.min(tabBar.width, tabBar.totalButtonWidth)

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

            function position() {
                if (!tabBar.relativeButtonPositions) return;
                print("woei")
                print(tabBar)
                print(tabBar.tabs)
                print(tabBar.tabs.selectedTabIndex)
                print(tabBar.relativeButtonPositions)
                offset = 1 - tabBar.relativeButtonPositions[tabBar.tabs.selectedTabIndex];
            }
        }
//    }

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
