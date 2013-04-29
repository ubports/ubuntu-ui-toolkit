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

Item {
    id: root
    width: 1000
    height: 200
    property var colors: ["lightblue", "blue", "darkblue", "navy"]
    property var widths: [50, 90, 50, 50]

    Item {
        id: listModel

        Repeater {
            model: colors.length
            delegate: Tab {
                title: colors[index]
            }
        }

//        Tab {
//            title: "lightblue"
//        }
//        Tab {
//            title: "green"
//        }
//        Tab {
//            title: "darkblue"
//        }
//        Tab {
//            title: "navy"
//        }
        Component.onCompleted: print(children.data)
    }

    property int firstIndex: 2
    Rectangle {
        id: startPosition
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 10
        color: "yellow"
        x: 10
    }

    property int numRects: colors.length*3


    Component {
        id: rectangle
        Rectangle {
            id: rect
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            //            width: widths[index % colors.length]
            //            color: "pink"
            color: modelData.title
            width: label.width + 20

            Label {
                id: label
                anchors.centerIn: parent
                text: modelData.title
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
//                    print("updating color")
//                    colors[0] = "yellow"
                    listModel.insert(1,{"col": "yellow", "name": "eeEe"})
                    print(listModel)
                }
            }

            Behavior on width {
                NumberAnimation { duration: 500 }
            }

            //            anchors.left: index === 0 ? startPosition.right : repeater.itemAt(index-1).right
        }
    }


//    Loader {
//        // needed to determine the width of the row.
//        // It won't be visible because "grey" is drawn over it.
//        id: loader
//        sourceComponent: row
//        anchors {
//            top: parent.top
//            bottom: parent.bottom
//            left: parent.left
//        }
//    }

//    Component {
//        id: row
        Row {
            anchors {
                top: parent.top
                bottom: parent.bottom
                topMargin: 5
                bottomMargin: 5
            }

            Repeater {
//                model: colors
                model: listModel.children
                delegate: rectangle
            }
        }
//    }


//    Rectangle {

//        Rectangle {
//            color: "black"
//            width: 10
//            anchors {
//                top: parent.top
//                bottom: parent.bottom
//                right: pathView.left
//            }
//        }

//        Rectangle {
//            color: "black"
//            width: 10
//            anchors {
//                top: parent.top
//                bottom: parent.bottom
//                left: pathView.right
//            }
//        }

//        clip: true
//        color: "grey"
//        id: grey
//        anchors.centerIn: parent
//        width: 700
//        height: parent.height

//        PathView {

//            highlightRangeMode: PathView.NoHighlightRange

//            id: pathView
//            anchors.fill: parent
//            model: 2
//            delegate: row

//            //            offset: 0.5
//            path: Path {
//                startX: 0 //startY: 100
//                //                PathQuad { x: 120; y: 25; controlX: 260; controlY: 75 }
//                //                PathQuad { x: 120; y: 100; controlX: -20; controlY: 75 }
//                PathLine {
//                    x: loader.width*2
//                }
//            }
//        }
//    }
}
