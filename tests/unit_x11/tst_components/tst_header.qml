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
//    onWidthChanged: print("root.width = "+width)

    Header {
//        width: parent.width
//        height: units.gu(6)

        flickable: flickable

        id: header
//        locked: lockedSwitch.checked
        Rectangle {
            anchors.fill: parent
            color: "red"
            opacity: 0.5
            border {
                color: "black"
                width: 2
            }
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
//        anchors.topMargin: units.gu(10)
        contentHeight: height * 2

        Grid {
            id: switchGrid
            columns: 2
            spacing: units.gu(1)
            anchors {
                top: parent.top
                left: parent.left
                margins: units.gu(5)
            }
            Switch {
                id: lockedSwitch
                checked: header.locked
                function trigger() {
                    header.locked = !header.locked;
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
        }
        Label {
            anchors {
                top : switchGrid.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: units.gu(6)
            }
            text: "Flick me"
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


    UbuntuTestCase {
        name: "Header"
        when: windowShown
        id: testCase

//        property var header
//        function initTestCase() {
//            testCase.header = findChild(mainView, "MainView_Header");
//        }

        function initTestCase() {
            compare(header.exposed, true, "Header not exposed initially");
//            compare(page.head.locked, false, "Header is not locked initially.");
        }

        function init() {
            flickable.contentHeight = 2*flickable.height;
            flickable.interactive = true;
            header.exposed = true;
            wait_for_exposed(true);
        }

        function scroll(dy) {
            var p = centerOf(flickable);
            // Use mouseWheel to scroll because mouseDrag is very unreliable
            // and does not properly handle negative values for dy.
            mouseWheel(flickable, p.x, p.y, 0,dy);
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
            header.height = undefined;
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
            header.height = undefined;
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

//            page.head.locked = true;
//            page.head.visible = false;
//            wait_for_visible(false, "Cannot hide locked header by setting visible to false.");
//            page.head.visible = true;
//            wait_for_visible(true, "Cannot show locked header by setting visible to true.");
        }

        function test_scroll_when_unlocked_updates_visible() {
            scroll_down();
            wait_for_exposed(false, "Scrolling down does not hide header.");
            scroll_up();
            wait_for_exposed(true, "Scrolling up does not show header.");
        }

//        function test_flickable_margins() {
//            // TODO TIM
//        }

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

//        function test_scroll_when_locked_does_not_update_visible() {
//            // Note that with a locked header, scrolling up and down does not
//            //  cause the header to move, so the wait_for_visible() calls below
//            //  will return almost instantly.
//            page.head.locked = true;
//            scroll_down();
//            wait_for_visible(true, "Scrolling down hides locked header.");
//            scroll_up();
//            wait_for_visible(true, "Scrolling up hides locked header.");

//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            scroll_down();
//            wait_for_visible(false, "Scrolling down shows locked header.");
//            scroll_up();
//            wait_for_visible(false, "Scrolling up shows locked header.");
//        }

//        function test_locking_updates_visible() {
//            // locked = false, visible = true.
//            page.head.locked = true;
//            wait_for_visible(true, "Locking hides header.");
//            page.head.locked = false;
//            wait_for_visible(true, "Unlocking hides header.");

//            page.head.locked = true;
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            // When the flickable is scrolled to the top, unlocking the header must show
//            //  the header because you cannot scroll more up to reveal it:
//            page.head.locked = false;
//            wait_for_visible(true, "Unlocking header when flickable is at Y beginning "+
//                             "does not show header.");

//            scroll_down();
//            wait_for_visible(false, "Scrolling down does not hide header.");
//            page.head.locked = true;
//            wait_for_visible(false, "Locking shows header.");
//            // When flickable is scrolled down, unlocking header does not show header
//            //  because the user can scroll up to reveal it:
//            page.head.locked = false;
//            wait_for_visible(false, "Unlocking shows header when flickable is not at " +
//                             "Y beginning.");
//        }

//        function test_activate_page_shows_header() {
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);

//            // Header becomes visible when new Page becomes active:
//            stack.push(otherPage);
//            wait_for_visible(true, "Pushing page on stack does not show header.");

//            // Header becomes visible when Page with previously hidden header
//            // becomes active:
//            stack.pop();
//            wait_for_visible(true, "Activating unlocked Page does not make header visible.");
//        }

//        function test_activate_hides_locked_hidden_header() {
//            otherPage.head.locked = true;
//            otherPage.head.visible = false;

//            stack.push(otherPage);
//            wait_for_visible(false, "Pushing Page with locked hidden header shows header.");
//            compare(otherPage.head.locked, true, "Pushing Page unlocks header.");
//            compare(page.head.locked, false, "Pushing Page locks previous header.");

//            stack.pop();
//            wait_for_visible(true, "Popping to a Page with unlocked header does not show header.");
//            compare(otherPage.head.locked, true, "Popping Page unlocks previous header.");
//            compare(page.head.locked, false, "Popping Page locks header.");
//        }

//        function test_hidden_locked_header_stays_hidden() {
//            page.head.locked = true;
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            stack.push(otherPage);
//            waitForHeaderAnimation(mainView);
//            stack.pop();
//            wait_for_visible(false, "Popping to a Page with locked hidden header shows header.");
//        }

//        function test_page_with_no_title_on_pagestack_has_back_button_bug1402054() {
//            page.head.visible = false;
//            waitForHeaderAnimation(mainView);
//            stack.push(titleLessPage);
//            wait_for_visible(true, "Page with no title hides the header.");

//            var backButton = findChild(testCase.header, "backButton");
//            verify(null !== backButton, "Header has no back button.");
//            compare(backButton.visible, true, "Page with no title hides the back button.");

//            var center = centerOf(backButton);
//            mouseClick(backButton, center.x, center.y);
//            waitForHeaderAnimation(mainView);
//            compare(stack.depth, 1, "Clicking back button of page with no title does not "+
//                    "pop the page from the PageStack.");
//        }

//        function test_disable_buttons_when_animating_header_bug1478147() {
//            stack.push(noHeaderPage);
//            stack.push(otherPage);
//            waitForHeaderAnimation(mainView);

//            var backButton = findChild(testCase.header, "backButton");
//            var center = centerOf(backButton);
//            mouseMove(backButton, center.x, center.y);

//            // Click the back button to pop the stack and go back to
//            //  noHeaderPage. This hides the header:
//            mouseClick(backButton, center.x, center.y);
//            // Second click, which does not generate an event because animating
//            //  of the header must disable its buttons:
//            mouseClick(backButton, center.x, center.y);
//            waitForHeaderAnimation(mainView);
//            // Compare the titles instead of the pages directly to avoid getting
//            //  a "Maximum call stack size exceeded." exception.
//            compare(stack.currentPage.title, noHeaderPage.title,
//                    "Back button in animating header was clicked.");

//            stack.pop(); // noHeaderPage
//            waitForHeaderAnimation(mainView);
//        }
    }
}
