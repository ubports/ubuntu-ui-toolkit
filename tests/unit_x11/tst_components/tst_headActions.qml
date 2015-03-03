/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1

// NOTE: Other parts of the page head API are tested with autopilot in
// ubuntuuitoolkit.tests.components.test_header

Item {
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainView
        anchors.fill: parent
        useDeprecatedToolbar: false

        PageStack {
            id: pageStack
            Page {
                id: page1
                title: "First page"
            }
            Page {
                id: page2
                title: "Second page"

                Action {
                    id: customBackAction
                    iconName: "search"
                    text: "Search"
                }
                Action {
                    id: invisibleAction
                    visible: false
                }
            }
        }
        Component.onCompleted: {
            pageStack.push(page1);
        }
    }

    UbuntuTestCase {
        name: "HeaderBackButton"
        when: windowShown
        id: testCase

        property var app_header
        property var back_button
        property var custom_back_button
        property var head_style

        function initTestCase() {
            testCase.app_header = findChild(mainView, "MainView_Header");
            testCase.back_button = findChild(app_header, "backButton");
            testCase.custom_back_button = findChild(app_header, "customBackButton");
            testCase.head_style = findChild(app_header, "PageHeadStyle");

            wait_for_animation();
            compare(page2.head.backAction, null, "Back action set by default.");
            compare(back_button.visible, false, "Back button visible with only 1 page on the stack.");
            compare(custom_back_button.visible, false, "Custom back button visible without custom back action.")
        }

        function wait_for_animation() {
            tryCompareFunction(function(){return testCase.head_style.animating}, false);
        }

        function test_default_back_button() {
            pageStack.push(page2);
            wait_for_animation();
            compare(back_button.visible, true, "Back button not visible with 2 pages on stack.");
            compare(custom_back_button.visible, false, "Showing custom back button without custom back action.");
            pageStack.pop();
            wait_for_animation();
        }

        function test_custom_back_button() {
            page2.head.backAction = customBackAction;
            pageStack.push(page2);
            wait_for_animation();
            compare(back_button.visible, false, "Default back button visible with custom back action.");
            compare(custom_back_button.visible, true, "Custom back button invisible with back action.");
            pageStack.pop();
            wait_for_animation();
            page2.head.backAction = null;
        }

        function test_no_back_button() {
            page2.head.backAction = invisibleAction;
            pageStack.push(page2);
            wait_for_animation();
            compare(back_button.visible, false, "Default back button visible with invisible custom back action.");
            compare(custom_back_button.visible, false, "Custom back button visible with invisible custom back action.");
            pageStack.pop();
            wait_for_animation();
            page2.head.backAction = null;
        }
    }
}
