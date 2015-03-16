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
import Ubuntu.Components 1.2
import Ubuntu.Test 1.0

Item {
    width: units.gu(50)
    height: units.gu(70)

    MainView {
        id: mainView
        width: units.gu(50)
        height: units.gu(70)

        PageStack {
            id: stack
            Component.onCompleted: stack.push(page)

            Page {
                id: page
                title: "Auto-hide"
                head {
                    locked: lockedSwitch.checked
                    onVisibleChanged: {
                        visibleSwitch.checked = page.head.visible
                    }
                }
                Flickable {
                    id: flickable
                    anchors.fill: parent
                    contentHeight: units.gu(200)
                    Grid {
                        id: switchGrid
                        columns: 2
                        spacing: units.gu(1)
                        anchors {
                            top: parent.top
                            left: parent.left
                            leftMargin: units.gu(5)
                            topMargin: units.gu(15)
                        }
                        Switch {
                            id: lockedSwitch
                            checked: false
                        }
                        Label {
                            text: "header locked"
                        }
                        Switch {
                            id: visibleSwitch
                            checked: page.head.visible
                            onClicked: page.head.visible = checked
                        }
                        Label {
                            text: "header visible"
                        }
                    }
                    Label {
                        id: label
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            top: switchGrid.bottom
                            topMargin: units.gu(15)
                        }
                        text: "Flick me"
                    }
                    Button {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            top: label.bottom
                            topMargin: units.gu(5)
                        }
                        text: "Click me"
                        onTriggered: stack.push(otherPage)
                    }
                }
            }
            Page {
                id: otherPage
                title: "On stack"
                visible: false

                Label {
                    anchors.centerIn: parent
                    text: "Stacked"
                }
            }
        }
    }

    UbuntuTestCase {
        name: "HeaderLockedVisible"
        when: windowShown
        id: testCase

        property var header
        property var headerStyle

        function initTestCase() {
            testCase.header = findChild(mainView, "MainView_Header");
            testCase.headerStyle = findChild(mainView, "PageHeadStyle");
        }

        function init() {
            page.head.visible = true;
            page.head.locked = false;
            otherPage.head.visible = true;
            otherPage.head.locked = false;
            testCase.wait_for_visible(true, "Header is not visible initially.");
            compare(stack.currentPage, page, "Wrong Page on PageStack initially.");
            compare(page.head.locked, false, "Header is not locked initially.");
        }

        function scroll(dy) {
            var p = centerOf(mainView);
            // Use mouseWheel to scroll because mouseDrag is very unreliable
            // and does not properly handle negative values for dy.
            mouseWheel(mainView, p.x, p.y, 0, 2*dy);
        }

        function scroll_down() {
            testCase.scroll(-header.height);
        }

        function scroll_up() {
            testCase.scroll(header.height);
        }

        function wait_for_animation() {
            // Wait for the header to start to move:
            wait(50);
            // Wait for animation of the style inside the header (when pushing/popping):
            tryCompareFunction(function(){ return testCase.headerStyle.animating }, false);
            // Wait for the header to finish showing/hiding
            tryCompareFunction(function(){ return testCase.header.moving }, false);
        }

        function wait_for_visible(visible, errorMessage) {
            wait_for_animation();
            compare(stack.currentPage.head.visible, visible, errorMessage);
            var mismatchMessage = " Page.head.visible does not match header visibility.";
            if (visible) {
                compare(header.y, 0, errorMessage + mismatchMessage);
            } else {
                compare(header.y, -header.height, errorMessage + mismatchMessage);
            }
        }

        function test_set_visible_to_hide_and_show() {
            page.head.visible = false;
            wait_for_visible(false, "Cannot hide unlocked header by setting visible to false.");
            page.head.visible = true;
            wait_for_visible(true, "Cannot show unlocked header by setting visible to true.");

            page.head.locked = true;
            page.head.visible = false;
            wait_for_visible(false, "Cannot hide locked header by setting visible to false.");
            page.head.visible = true;
            wait_for_visible(true, "Cannot show locked header by setting visible to true.");
        }

        function test_scroll_when_unlocked_updates_visible() {
            testCase.scroll_down();
            testCase.wait_for_visible(false, "Scrolling down does not hide header.");
            testCase.scroll_up();
            testCase.wait_for_visible(true, "Scrolling up does not show header.");
        }

        function test_scroll_when_locked_does_not_update_visible() {
            // Note that with a locked header, scrolling up and down does not
            //  cause the header to move, so the wait_for_visible() calls below
            //  will return almost instantly.
            page.head.locked = true;
            scroll_down();
            wait_for_visible(true, "Scrolling down hides locked header.");
            scroll_up();
            wait_for_visible(true, "Scrolling up hides locked header.");

            page.head.visible = false;
            wait_for_animation();
            scroll_down();
            wait_for_visible(false, "Scrolling down shows locked header.");
            scroll_up();
            wait_for_visible(false, "Scrolling up shows locked header.");
        }

        function test_locking_updates_visible() {
            // locked = false, visible = true.
            page.head.locked = true;
            wait_for_visible(true, "Locking hides header.");
            page.head.locked = false;
            wait_for_visible(true, "Unlocking hides header.");

            page.head.locked = true;
            page.head.visible = false;
            wait_for_animation();
            // When the flickable is scrolled to the top, unlocking the header must show
            //  the header because you cannot scroll more up to reveal it:
            page.head.locked = false;
            wait_for_visible(true, "Unlocking header when flickable is at Y beginning "+
                             "does not show header.");

            scroll_down();
            wait_for_visible(false, "Scrolling down does not hide header.");
            page.head.locked = true;
            wait_for_visible(false, "Locking shows header.");
            // When flickable is scrolled down, unlocking header does not show header
            //  because the user can scroll up to reveal it:
            page.head.locked = false;
            wait_for_visible(false, "Unlocking shows header when flickable is not at " +
                             "Y beginning.");
        }

        function test_activate_page_shows_header() {
            page.head.visible = false;
            wait_for_animation();

            // Header becomes visible when new Page becomes active:
            stack.push(otherPage);
            wait_for_visible(true, "Pushing page on stack does not show header.");

            // Header becomes visible when Page with previously hidden header
            // becomes active:
            stack.pop();
            wait_for_visible(true, "Activating unlocked Page does not make header visible.");
        }

        function test_activate_hides_locked_hidden_header() {
            otherPage.head.locked = true;
            otherPage.head.visible = false;

            stack.push(otherPage);
            wait_for_visible(false, "Pushing Page with locked hidden header shows header.");
            compare(otherPage.head.locked, true, "Pushing Page unlocks header.");
            compare(page.head.locked, false, "Pushing Page locks previous header.");

            stack.pop();
            wait_for_visible(true, "Popping to a Page with unlocked header does not show header.");
            compare(otherPage.head.locked, true, "Popping Page unlocks previous header.");
            compare(page.head.locked, false, "Popping Page locks header.");
        }

        function test_hidden_locked_header_stays_hidden() {
            page.head.locked = true;
            page.head.visible = false;
            wait_for_animation();
            stack.push(otherPage);
            wait_for_animation();
            stack.pop();
            wait_for_visible(false, "Popping to a Page with locked hidden header shows header.");
        }
    }
}
