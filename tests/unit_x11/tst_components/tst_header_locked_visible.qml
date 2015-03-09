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

        function initTestCase() {
            testCase.check_header_visibility(true);
        }

        function scroll_down() {
            // TODO
        }

        function scroll_up() {
            // TODO
        }

        function check_header_visibility(visible) {
            // TODO
            // wait for animation
            // check value of Page.head.visible
            // check actual visibility
        }

        function test_visible_to_hide_and_show() {
            page.head.visible = false;
            testCase.check_header_visibility(false);
            page.head.visible = true;
            testCase.check_header_visibility(true);

            // ensure that setting visible also works
            // when the header is locked:
            page.head.locked = true;
            page.head.visible = false;
            testCase.check_header_visibility(false);
            page.head.visible = true;
            testCase.check_header_visibility(true);
        }

        function test_scroll_to_hide_and_show() {
            testCase.scroll_down();
            testCase.check_header_visibility(false);
            testCase.scroll_up();
            testCase.check_header_visibility(true);
        }

        function test_dont_hide_when_locked() {
            page.head.locked = true;
            testCase.scroll_down();
            // header did not auto-hide when locked:
            testCase.check_header_visibility(true);
        }

        function test_dont_show_when_locked() {
            testCase.scroll_down();
            testCase.check_header_visibility(false);
            page.head.locked = true;
            testCase.scroll_up();
            // header did not auto-show when locked:
            testCase.check_header_visibility(false);
        }

        function test_set_locked_and_visible_independently() {
            // visible is true, locked is false

            // locking does not update visible:
            page.head.locked = true;
            testCase.check_header_visibility(true);

            // hiding does not update locked:
            page.head.visible = false;
            // TODO: check that locked is true

            // unlocking does not update visible:
            page.head.locked = false;
            testCase.check_header_visibility(false);

            // showing does not update locked:
            page.head.visible = true;
            // TODO: check that locked is false
        }
    }
}
