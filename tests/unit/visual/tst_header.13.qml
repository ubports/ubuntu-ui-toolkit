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
import Ubuntu.Test 1.0

Item {
    id: root
    width: units.gu(50)
    height: units.gu(70)

    property real initialHeaderHeight: units.gu(6)

    Header {
        id: header
        flickable: flickable
        z: 1
        width: parent ? parent.width : 234
        height: root.initialHeaderHeight

        Rectangle {
            // to visualize the header
            anchors.fill: parent
            color: UbuntuColors.red
            opacity: 0.5
            border {
                color: "black"
                width: 2
            }
        }

        onMovingChanged: {
            // The value of movingLabel.text is used in:
            // - test_dont_move_when_flickable_shortens_bug1514143()
            // - test_dont_move_exposed_header_when_scrolling_down_bug1514143()
            // - test_dont_move_hidden_header_when_scrolling_up()
            movingLabel.text = "Moving changed to " + moving;
            movingLabel.color = moving ? "purple" : "red";
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
            id: flickLabel
            anchors {
                top: contentYButton.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: units.gu(8)
            }
            text: "Flick me"
        }
        Label {
            id: movingLabel
            anchors {
                top: flickLabel.bottom
                horizontalCenter: parent.horizontalCenter
            }
            text: "hmm"
        }
    }

    Rectangle {
        id: reparentTestItem
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        height: units.gu(15)
        width: parent.width / 2
        color: "blue"

        Label {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }
            text: "Click to reparent"
            color: "white"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (header.parent === root) {
                    header.parent = reparentTestItem;
                } else {
                    header.parent = root;
                }
            }
        }
    }

    Flickable {
        id: otherFlickable
        property real initialTopMargin: 123
        topMargin: initialTopMargin
        height: units.gu(10)
        contentHeight: units.gu(20);
    }

    Header {
        id: otherHeader
    }

    Header {
        id: hiddenHeader
        exposed: false
        height: root.initialHeaderHeight
    }

    UbuntuTestCase {
        name: "Header"
        when: windowShown
        id: testCase

        function initTestCase() {
            wait_for_exposed(true, "Header is not exposed initially.");
            compare(otherHeader.flickable, null, "Flickable not null by default.");
            compare(header.flickable, flickable, "Flickable not properly intialized.");
            // note: moving may be true briefly due to header height changes, but
            //  it does not change in the initialization after wait_for_exposed() above.
            compare(header.moving, false, "Header moving initially.");
        }

        function init() {
            header.flickable = flickable;
            flickable.contentHeight = 2*flickable.height;
            flickable.interactive = true;
            flickable.contentY = -header.height;
            header.exposed = true;
            wait_for_exposed(true);
        }

        function scroll(dy) {
            var p = centerOf(flickable);
            // Use mouseWheel to scroll because mouseDrag is very unreliable
            // and does not properly handle negative values for dy.
            mouseWheel(flickable, p.x, p.y, 0, dy);
        }

        function scroll_down() {
            scroll(-2.0*header.height);
        }

        function scroll_up() {
            scroll(header.height);
        }

        function wait_for_exposed(exposed, errorMessage) {
            tryCompare(header, "exposed", exposed, 5000, errorMessage);
            // wait for the animation to finish:
            tryCompare(header, "moving", false, 5000, "Header still moving?");
            if (exposed) {
                compare(header.y, 0, errorMessage +
                        " y-value/exposed mismatch for exposed header!");
            } else {
                compare(header.y, -header.height, errorMessage +
                        " y-value/exposed mismatch for hidden header!");
            }
        }

        function test_0_initially_hidden() {
            // Don't show an animation if the header is hidden initially.
            compare(hiddenHeader.y, -header.height,
                    "Hidden header has wrong initial y-value.");
        }

        function test_reparent_width() {
            // test initial header width:
            compare(header.parent, root);
            compare(header.width, root.width);
            compare(header.y, 0);

            // test width update when changing parent:
            header.parent = reparentTestItem;
            compare(header.parent, reparentTestItem);
            compare(header.width, reparentTestItem.width);
            compare(header.y, 0);

            // test width update when changing width of parent:
            var old_width = reparentTestItem.width;
            reparentTestItem.width = units.gu(5);
            compare(header.width, reparentTestItem.width);
            compare(header.y, 0);

            // revert to original parent:
            header.parent = root;
            compare(header.parent, root);
            compare(header.width, root.width);
            compare(header.y, 0);
            reparentTestItem.width = old_width;
        }

        function test_height_change() {
            // first scroll down for this test, the following test repeats
            //  the cases at the top of the flickable.
            scroll_down();
            header.height = units.gu(15);
            wait_for_exposed(true, "Increasing header height hides header.");
            scroll_down();
            header.exposed = false;
            wait_for_exposed(false, "Header with height set does not hide.");
            header.height = units.gu(2);
            wait_for_exposed(false, "Decreasing header height exposes it.");
            header.exposed = true;
            wait_for_exposed(true, "Header with decreased height does not expose.");

            // revert to initial state
            header.height = root.initialHeaderHeight;
            flickable.contentY = -header.height;
            wait_for_exposed(true, "Setting flickable.contentY hides the header.");
        }

        function test_height_change_at_top() {
            // Near the top, changing the header height exposes the header
            //  to avoid the header becoming inaccessible because it cannot
            //  be pulled down.
            header.exposed = false;
            wait_for_exposed(false);
            header.height = units.gu(15);
            wait_for_exposed(true, "Increasing header height at top hides header.");

            // making the header smaller does not need to expose it, because there is
            //  enough space to pull it down.

            // revert to original state.
            header.height = root.initialHeaderHeight;
            flickable.contentY = -header.height;
            header.exposed = true;
            wait_for_exposed(true);
        }

        function test_set_exposed_to_hide_and_show() {
            header.exposed = false;
            wait_for_exposed(false, "Cannot hide header by setting visible to false.");
            header.exposed = true;
            wait_for_exposed(true, "Cannot show header by setting visible to true.");

            // change the value of exposed twice quickly:
            header.exposed = false;
            header.exposed = true;
            wait_for_exposed(true, "Quickly hiding and showing header does not result in exposed header.");

            // and the other way around:
            header.exposed = false;
            wait_for_exposed(false);
            header.exposed = true;
            header.exposed = false;
            wait_for_exposed(false, "Quickly showing and hiding header does not result in hidden header.");

            header.exposed = true;
            wait_for_exposed(true);
        }

        function test_scroll_updates_exposed() {
            scroll_down();
            wait_for_exposed(false, "Scrolling down does not hide header.");
            scroll_up();
            wait_for_exposed(true, "Scrolling up does not show header.");
        }

        function test_flickable_margins() {
            compare(flickable.topMargin, header.height, "Flickable top margin does not match header height.");
            header.height = units.gu(15);
            wait_for_exposed(true, "Increasing header height at top hides header.");
            compare(flickable.topMargin, header.height, "Updating header height does not update flickable top margin.");

            header.height = root.initialHeaderHeight; // revert
            wait_for_exposed(true, "Reverting header height at top hides header.");
            compare(flickable.topMargin, header.height, "Reverting header height does not revert flickable top margin.");

            compare(otherFlickable.topMargin, otherFlickable.initialTopMargin, "Flickable top margin is not initialized properly.");
            header.flickable = otherFlickable;
            compare(otherFlickable.topMargin, header.height + otherFlickable.initialTopMargin,
                    "Setting flickable does not update flickable top margin.");
            compare(flickable.topMargin, 0,
                    "Changing the flickable does not reset the previous flickable top margin to 0.");

            header.flickable = flickable;
            compare(otherFlickable.topMargin, otherFlickable.initialTopMargin, "Reverting flickable does not restore the other flickable top margin.");
            compare(flickable.topMargin, header.height, "Reverting flickable breaks flickable top margin.");
        }

        function test_flickable_margins_invisible_header_bug1560458_bug1560419_data() {
            return [
                        { tag: "Inititial topMargin = 0",
                            flickable: flickable,
                            initialTopMargin: 0
                        },
                        { tag: "Initial topMargin > 0",
                            flickable: otherFlickable,
                            initialTopMargin: otherFlickable.initialTopMargin
                        }
                    ];
        }

        function test_flickable_margins_invisible_header_bug1560458_bug1560419(data) {
            var h = header.height;
            var fl = data.flickable;
            // the original margin before the flickable is connected to the header
            var fmargin = data.initialTopMargin;
            if (header.flickable !== fl) {
                compare(fl.topMargin, fmargin,
                        "Incorrect initial topMargin for flickable.");
                header.flickable = fl;
            }

            compare(fl.topMargin, h + fmargin,
                    "Flickable top margin does not match header height.");
            header.visible = false;
            compare(fl.topMargin, fmargin,
                    "Invisible header sets flickable.topMargin.");
            header.visible = true;
            compare(fl.topMargin, h + fmargin,
                    "Making flickable visible does not set flickable.topMargin.");
            header.height = 0;
            compare(fl.topMargin, fmargin,
                    "Header with height = 0 sets flickable.topMargin.");
            header.height = h;
            compare(fl.topMargin, h + fmargin,
                    "Setting header height does not set flickable.topMargin.");

            // Setting opacity to 0 should not change flickable.topMargin.
            // This allows opacity animations.
            header.opacity = 0.0;
            compare(fl.topMargin, h + fmargin,
                    "Setting header opacity to 0 changes flickable.topMargin.");
            header.opacity = 1.0;
            compare(fl.topMargin, h + fmargin,
                    "Setting header opacity to 1 changes flickable.topMargin.");

            header.parent = null;
            compare(fl.topMargin, fmargin,
                    "Header with no parent sets flickable.topMargin.");
            header.parent = root;
            compare(fl.topMargin, h + fmargin,
                    "Header with parent does not set flickable.topMargin.");
        }

        function test_flickable_contentHeight_bug1156573() {
            var old_height = flickable.contentHeight;
            header.exposed = false;
            wait_for_exposed(false);
            flickable.contentHeight = flickable.height / 2;
            wait_for_exposed(true, "Small content height does not expose the header.");

            // revert:
            flickable.contentHeight = old_height;
            compare(header.exposed, true, "Reverting flickable content height hides the header.");
        }

        function test_flickable_interactive() {
            header.exposed = false;
            wait_for_exposed(false);
            flickable.interactive = false;
            wait_for_exposed(true, "Making flickable not interactive does not expose the header.");

            // revert:
            flickable.interactive = true;
            compare(header.exposed, true, "Reverting flickable exposed hides the header.");
        }

        function test_scroll_disconnected_flickable() {
            var hy = header.y;
            header.flickable = null;
            scroll_down();
            compare(header.y, hy, "Header scrolls when disconnected flickable scrolls down.");
            wait_for_exposed(true, "Scrolling down disconnected flickable hides header.");
            scroll_up();
            compare(header.y, hy, "Header scrolls when disconnected flickable scrolls up.");
            wait_for_exposed(true, "Scrolling up disconnected flickable hides header.");

            header.flickable = flickable;
        }

        function test_dont_move_when_flickable_shortens_bug1514143() {
            var flickableContentHeight = flickable.contentHeight;
            movingLabel.text = "HEADER DID NOT MOVE";
            flickable.contentHeight = 200;
            compare(movingLabel.text, "HEADER DID NOT MOVE",
                    "Reducing flickable contents height unneccessary sets header.moving.");
            flickable.contentHeight = flickableContentHeight;
            compare(movingLabel.text, "HEADER DID NOT MOVE",
                    "Increasing flickable contents height unneccessary sets header.moving.");
        }

        function test_dont_move_exposed_header_when_scrolling_down_bug1514143() {
            scroll_down(); scroll_down();
            wait_for_exposed(false, "Header doesn't hide when scrolling down.");
            header.exposed = true;
            wait_for_exposed(true, "Cannot expose header after scrolling down.");
            movingLabel.text = "HEADER DID NOT MOVE";
            scroll_up();
            wait(100);
            compare(movingLabel.text, "HEADER DID NOT MOVE",
                    "Header moved when scrolling up while header was already exposed.");
        }

        function test_dont_move_hidden_header_when_scrolling_up() {
            // flickable is at the top.
            header.exposed = false;
            wait_for_exposed(false, "Cannot hide header.");
            movingLabel.text = "HEADER DID NOT MOVE";
            scroll_down();
            wait(100);
            compare(movingLabel.text, "HEADER DID NOT MOVE",
                    "Header moved when scrolling down while header was already hidden.");
        }
    }
}
