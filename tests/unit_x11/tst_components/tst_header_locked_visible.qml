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
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        top: switchGrid.bottom
                        topMargin: units.gu(15)
                    }
                    text: "Flick me"
                }
            }
        }
    }

    UbuntuTestCase {
        name: "HeaderLockedVisible"
        when: windowShown
        id: testCase

        property var header
        function initTestCase() {
            testCase.header = findChild(mainView, "MainView_Header");
            testCase.wait_for_animation();
        }

        function init() {
            testCase.check_header_visibility(true, "Header is not visible initially.");
            compare(page.head.locked, false, "Header is not locked initially.");
        }
        function cleanup() {
            page.head.visible = true;
            page.head.locked = false;
            testCase.wait_for_animation();
        }

        function scroll(dy) {
            var x = flickable.width / 2;
            var y = flickable.height / 2;
            mouseDrag(flickable, x, y, 0, dy);
        }

        function scroll_down() {
            testCase.scroll(-header.height);
        }

        function scroll_up() {
            testCase.scroll(header.height);
        }

        function wait_for_animation() {
            tryCompareFunction(function(){return testCase.header.moving}, false);
        }

        function check_header_visibility(visible, errorMessage) {
            testCase.wait_for_animation();
            compare(page.head.visible, visible, errorMessage);
            if (visible) {
                compare(header.y, 0, errorMessage +
                        " Page.head.visible does not match header visibility.");
            } else {
                compare(header.y, -header.height, errorMessage +
                        " Page.head.visible does not match header visibility.");
            }
        }

        function test_visible_to_hide_and_show() {
            page.head.visible = false;
            testCase.check_header_visibility(false, "Setting Page.head.visible to "+
                                             "false does not hide the header.");
            page.head.visible = true;
            testCase.check_header_visibility(true, "Setting Page.head.visible to "+
                                             "true does not show the header.");

            // ensure that setting visible also works
            // when the header is locked:
            page.head.locked = true;
            page.head.visible = false;
            testCase.check_header_visibility(false, "Setting Page.head.visible to false "+
                                             "does not hide locked header.");
            page.head.visible = true;
            testCase.check_header_visibility(true, "Setting Page.head.visible to true "+
                                             "does not show locked header");
        }

        function test_scroll_to_hide_and_show() {
            testCase.scroll_down();
            testCase.check_header_visibility(false, "Scrolling down does not hide header.");
            testCase.scroll_up();
            testCase.check_header_visibility(true, "Scrolling up does not show header.");
        }

        function test_dont_hide_when_locked() {
            page.head.locked = true;
            testCase.check_header_visibility(true, "Locking hides the header.");
            testCase.scroll_down();
            // header did not auto-hide when locked:
            testCase.check_header_visibility(true, "Scrolling down hides locked header.");
        }

        function test_dont_show_when_locked() {
            testCase.scroll_down();
            testCase.check_header_visibility(false, "Scrolling down does not hide header.");
            page.head.locked = true;
            testCase.check_header_visibility(false, "Locking shows the header.")
            testCase.scroll_up();
            // header did not auto-show when locked:
            testCase.check_header_visibility(false, "Scrolling up shows locked header.");
        }

        function test_set_locked_and_visible_independently() {
            // visible is true, locked is false

            // locking does not update visible:
            page.head.locked = true;
            testCase.check_header_visibility(true, "Locking hides header.");

            // hiding does not update locked:
            page.head.visible = false;
            compare(page.head.locked, true, "Setting page.head.visible locks header.");

            // unlocking does not update visible:
            page.head.locked = false;
            testCase.check_header_visibility(false, "Unlocking header hides it.");

            // showing does not update locked:
            page.head.visible = true;
            compare(page.head.locked, false, "Showing header unlocks it.");
        }
    }
}
