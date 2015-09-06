/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

// NOTE: Other parts of the page head API are tested with autopilot in
// ubuntuuitoolkit.tests.components.test_header

Item {
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainView
        anchors.fill: parent

        PageStack {
            id: pageStack
            Page {
                id: page1
                title: "First page"

                Button {
                    anchors.centerIn: parent
                    onClicked: pageStack.push(page2)
                    text: "Push page 2"
                }

                head {
                    actions: [
                        Action {
                            iconName: "settings"
                            text: "first"
                            onTriggered: print("Trigger first action")
                        },
                        Action {
                            iconName: "info"
                            text: "second"
                            onTriggered: print("Trigger second action")
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
                }
            }
            Page {
                id: page2
                visible: false
                title: "Second page"

                Action {
                    id: customBackAction
                    iconName: "close"
                    text: "Close"
                    onTriggered: print("Triggered custom back action.")
                }
                Action {
                    id: invisibleAction
                    visible: false
                }
                head {
                    backAction: customBackActionSwitch.checked ?
                                    customBackAction : null
                }

                Row {
                    anchors.centerIn: parent
                    spacing: units.gu(1)
                    Label {
                        text: "standard"
                    }
                    Switch {
                        id: customBackActionSwitch
                    }
                    Label {
                        text: "custom back action"
                    }
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

        property var back_button
        property var custom_back_button

        function initTestCase() {
            var app_header = findChild(mainView, "MainView_Header");
            testCase.back_button = findChild(app_header, "backButton");
            testCase.custom_back_button = findChild(app_header, "customBackButton");

            waitForHeaderAnimation(mainView);
            compare(page2.head.backAction, null, "Back action set by default.");
            compare(back_button.visible, false, "Back button visible with only 1 page on the stack.");
            compare(custom_back_button.visible, false, "Custom back button visible without custom back action.")
        }

        function test_default_back_button() {
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);
            compare(back_button.visible, true, "Back button not visible with 2 pages on stack.");
            compare(custom_back_button.visible, false, "Showing custom back button without custom back action.");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
        }

        function test_custom_back_button() {
            page2.head.backAction = customBackAction;
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);
            compare(back_button.visible, false, "Default back button visible with custom back action.");
            compare(custom_back_button.visible, true, "Custom back button invisible with back action.");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            page2.head.backAction = null;
        }

        function test_no_back_button() {
            page2.head.backAction = invisibleAction;
            pageStack.push(page2);
            waitForHeaderAnimation(mainView);
            compare(back_button.visible, false, "Default back button visible with invisible custom back action.");
            compare(custom_back_button.visible, false, "Custom back button visible with invisible custom back action.");
            pageStack.pop();
            waitForHeaderAnimation(mainView);
            page2.head.backAction = null;
        }
    }
}
