/*
 * Copyright (C) 2015 Canonical Ltd.
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
//import Ubuntu.Test 1.0

Item {
    id: root
    width: units.gu(50)
    height: units.gu(70)

    property real initialHeaderHeight: units.gu(6)

    PageHeader {
        id: header
        flickable: flickable
        z:1

        title: "kiwi"

//        StyleHints {
//            backgroundColor: "yellow"
//        }
//        contents: Rectangle {
//            color: "pink"
//            anchors.fill: parent
//            opacity: 0.5
//        }

        Rectangle {
            id: pinkRectangle
            color: "pink"
            anchors.fill: parent
            opacity: 0.5
        }

        trailingActionBar {

            actions: [
                Action {
                    iconName: "settings"
                    text: "first"
                    onTriggered: {
                        print("Trigger first action");
                        header.contents = pinkRectangle;
                    }
                },
                Action {
                    iconName: "info"
                    text: "second"
                    onTriggered: {
                        print("Trigger second action");
                        header.contents = null;
                    }
                },
                Action {
                    iconName: "search"
                    text: "third"
                    onTriggered: print("Trigger third action")
                },
                Action {
                    iconName: "appointment"
                    text: "fourth"
                    onTriggered: print("Trigger fourth action")
                }
            ]
//            delegate: Rectangle {
//                width: units.gu(5)
//                height: units.gu(5)
//                color: "pink"
////                opacity: 0.5
//                MouseArea {
//                    anchors.fill: parent
//                    onClicked: modelData.trigger()
//                }
//            }
        }
    }

    Flickable {
        id: flickable
        anchors {
            top: header.flickable ? parent.top : header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentHeight: root.height * 2

        Grid {
            id: switchGrid
            columns: 2
            spacing: units.gu(1)
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: units.gu(5)
                topMargin: 2*root.initialHeaderHeight
            }
            Switch {
                id: lockedSwitch
                checked: null === header.flickable
                function trigger() {
                    if (header.flickable) {
                        header.flickable = null;
                    } else {
                        header.flickable = flickable;
                    }
                }
            }
            Label {
                text: "header locked"
            }
            Switch {
                id: hiddenSwitch
                checked: header.exposed
                function trigger() {
                    header.exposed = !header.exposed;
                }
            }
            Label {
                text: "header exposed"
            }
            Item {
                width: 1
                height: 1
            }
        }
        Button {
            id: contentYButton
            anchors {
                top: switchGrid.bottom
                topMargin: units.gu(4)
                horizontalCenter: parent.horizontalCenter
            }
            property real newY: flickable.contentY == 0 ? -header.height : 0
            onClicked: flickable.contentY = newY;
            text: "Set contentY to " + newY
        }
        Label {
            anchors {
                top : contentYButton.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: units.gu(8)
            }
            text: "Flick me"
        }
    }


    //    UbuntuTestCase {
    //        name: "PageHeader"
    //        when: windowShown
    //        id: testCase

    //        function initTestCase() {
    //            // TODO
    //        }

    //        function init() {
    //            // TODO
    //        }

    //        function scroll(dy) {
    //            var p = centerOf(flickable);
    //            // Use mouseWheel to scroll because mouseDrag is very unreliable
    //            // and does not properly handle negative values for dy.
    //            mouseWheel(flickable, p.x, p.y, 0,dy);
    //        }

    //        function scroll_down() {
    //            scroll(-2.0*header.height);
    //        }

    //        function scroll_up() {
    //            scroll(header.height);
    //        }

    //        function wait_for_exposed(exposed, errorMessage) {
    //            tryCompare(header, "exposed", exposed, 5000, errorMessage);
    //            // wait for the animation to finish:
    //            tryCompare(header, "moving", false, 5000, "Header still moving?");
    //            if (exposed) {
    //                compare(header.y, 0, errorMessage +
    //                        " y-value/exposed mismatch for exposed header!");
    //            } else {
    //                compare(header.y, -header.height, errorMessage +
    //                        " y-value/exposed mismatch for hidden header!");
    //            }
    //        }
    //    }
}
