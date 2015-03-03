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
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

UbuntuTestCase {
    name: "PageHeadColorsAPI"
    id: testCase

    MainView {
        id: mainView
        width: units.gu(50)
        height: units.gu(80)

        useDeprecatedToolbar: false

        Page {
            id: page
            title: "Color test"
            head {
                actions: Action {
                    objectName: "example_action"
                    iconName: "settings"
                    text: "Settings"
                }
                backAction: Action {
                    iconName: "cancel"
                    text: "Cancel"
                }
            }
        }
    }

    property var header
    function initTestCase() {
        compare(mainView.active, true, "MainView always active.");
        compare(page.active, true, "Single page is active in MainView.");
        testCase.header = findChild(mainView, "MainView_Header");
    }

    function get_back_button() {
        return findChild(testCase.header, "backButton");
    }

    function get_custom_back_button() {
        return findChild(testCase.header, "customBackButton");
    }

    function get_action_button(actionName) {
        return findChild(testCase.header, actionName + "_header_button");
    }

    function get_actions_overflow_button() {
        return findChild(testCase.header, "actions_overflow_button");
    }

    function get_tabs_button() {
        return findChild(testCase.header, "tabsButton");
    }

    function get_title_label() {
        return findChild(testCase.header, "header_title_label");
    }

    function test_title_color() {
        var title = get_title_label();
        var test_color = "#123456";
        page.head.foregroundColor = test_color;
        compare(title.color, test_color, "Page head foreground color does not set title color.");
    }

    function test_button_colors() {
        var test_color = "#654321";
        page.head.foregroundColor = test_color;
        compare(get_back_button().color, test_color, "Back button color does not match header foreground color.");
        compare(get_custom_back_button().color, test_color, "Custom back button color does not match header foreground color.");
        compare(get_actions_overflow_button().color, test_color, "Actions overflow button color does not match header foreground color.");
        compare(get_tabs_button().color, test_color, "Tabs button color does not match header foreground color.");
        compare(get_action_button("example_action").color, test_color, "Action button color does not match header foreground color.");
    }
}
