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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Item {
    width: units.gu(40)
    height: units.gu(71)

    Loader {
        id: testLoader
        anchors.fill: parent
    }

    Component {
        id: inactiveActionContextInChain

        Item {
            anchors.fill: parent
            ActionContext {
                active: true
            }
            Item {
                anchors.fill: parent
                ActionContext {
                    objectName: "testContext"
                    active: true
                }

                ActionItem {
                    objectName: "testActionItem"
                    action: Action {
                        text: "Test"
                        shortcut: 'Ctrl+T'
                    }
                }
            }
        }
    }

    Component {
        id: ambiguiousShortcutsInSameContext
        Item {
            ActionContext {
                active: true
            }
            Column {
                ActionItem {
                    action: Action {
                        shortcut: "Ctrl+T"
                    }
                }
                ActionItem {
                    action: Action {
                        shortcut: "Ctrl+T"
                    }
                }
            }
        }
    }

    Component {
        id: ambiguiousShortcutsInDifferentContexts
        Item {
            id: root
            anchors.fill: parent
            ActionContext {
                active: true
            }
            Row {
                Item {
                    width: root.width / 2
                    height: root.height
                    ActionContext {
                        active: true
                    }
                    ActionItem {
                        action: Action {
                            shortcut: "Ctrl+T"
                        }
                    }
                }
                Item {
                    width: root.width / 2
                    height: root.height
                    ActionContext {
                        active: true
                    }
                    ActionItem {
                        action: Action {
                            shortcut: "Ctrl+T"
                        }
                    }
                }
            }
        }
    }

    Component {
        id: onePopupContextActive
        Item {
            PopupContext {
                objectName: "popup1"
                active: true
            }
            PopupContext {
                objectName: "popup2"
            }
            PopupContext {
                objectName: "popup3"
            }
        }
    }

    Component {
        id: popupTest
        MainView {
            id: main
            anchors.fill: parent
            applicationName: "testApp"

            property Popover popover: null

            Button {
                id: button
                objectName: "mainButton"
                anchors.centerIn: parent
                action: Action {
                    objectName: "mainAction"
                    text: "Test &button"
                    shortcut: "Ctrl+T"
                }
                onClicked: main.popover = PopupUtils.open(popup, button)
            }

            Component {
                id: popup
                Popover {
                    contentWidth: units.gu(30)
                    contentHeight: units.gu(30)
                    Button {
                        objectName: "popupButton"
                        action: Action {
                            objectName: "popupAction"
                            text: "Test &button"
                            shortcut: "Ctrl+T"
                        }
                    }
                }
            }
        }
    }

    Component {
        id: dialogTest
        MainView {
            id: main
            anchors.fill: parent
            applicationName: "testApp"

            property Dialog popover: null

            Button {
                id: button
                objectName: "mainButton"
                anchors.centerIn: parent
                action: Action {
                    objectName: "mainAction"
                    text: "Test &button"
                    shortcut: "Ctrl+T"
                }
                onClicked: main.popover = PopupUtils.open(dialog)
            }

            Component {
                id: dialog
                Dialog {
                    contentWidth: units.gu(30)
                    contentHeight: units.gu(30)
                    Button {
                        objectName: "popupButton"
                        action: Action {
                            objectName: "popupAction"
                            text: "Test &button"
                            shortcut: "Ctrl+T"
                        }
                    }
                }
            }
        }
    }

    UbuntuTestCase {
        name: "ContextualActions"
        when: windowShown

        SignalSpy {
            id: triggeredSpy
            signalName: "triggered"
        }

        function createTest(component) {
            testLoader.sourceComponent = component;
            tryCompareFunction(function() { return testLoader.item != null }, true, 1000);
            waitForRendering(testLoader.item);
            wait(200)
            return testLoader.item;
        }

        function cleanup() {
            testLoader.sourceComponent = null;
            triggeredSpy.target = null;
            triggeredSpy.clear();
            wait(200);
        }

        function test_inactive_actioncontext_in_chain() {
            var item = createTest(inactiveActionContextInChain);
            var testContext = findInvisibleChild(item, "testContext");
            verify(testContext);
            var testActionItem = findInvisibleChild(item, "testActionItem");
            verify(testActionItem);

            testContext.active = true;
            triggeredSpy.target = testActionItem.action;
            keyPress(Qt.Key_T, Qt.ControlModifier);
            triggeredSpy.wait(200);

            testContext.active = false;
            triggeredSpy.clear();
            keyPress(Qt.Key_T, Qt.ControlModifier);
            expectFailContinue("", "No trigger when a context is inactive");
            triggeredSpy.wait(200);
        }

        function test_ambiguous_actions_when_multiple_contexts_active_data() {
            return [
                {tag: "within same ActionContext", test: ambiguiousShortcutsInSameContext, message: warningFormat(66, 29, "QML Action: Ambiguous shortcut: Ctrl+T")},
                {tag: "within different ActionContexts", test: ambiguiousShortcutsInDifferentContexts, message: warningFormat(107, 33, "QML Action: Ambiguous shortcut: Ctrl+T")},
            ];
        }
        function test_ambiguous_actions_when_multiple_contexts_active(data) {
            var test = createTest(data.test);
            ignoreWarning(data.message);
            keyClick(Qt.Key_T, Qt.ControlModifier);
        }

        function test_one_popup_context_active() {
            var test = createTest(onePopupContextActive);
            var popup1 = findInvisibleChild(test, "popup1");
            var popup2 = findInvisibleChild(test, "popup2");
            var popup3 = findInvisibleChild(test, "popup3");
            verify(popup1);
            verify(popup2);
            verify(popup3);
            verify(popup1.active);
            verify(!popup2.active);
            verify(!popup3.active);

            // activate popup2
            popup2.active = true;
            verify(!popup1.active);
            verify(popup2.active);
            verify(!popup3.active);

            // activate popup2
            popup3.active = true;
            verify(!popup1.active);
            verify(!popup2.active);
            verify(popup3.active);

            // deactivate popup3, popup2 should be re-activated
            popup3.active = false;
            verify(!popup1.active);
            verify(popup2.active);
        }

        function test_popovers_data() {
            return [
                {tag: "Popup", component: popupTest},
                {tag: "Dialog", component: dialogTest},
            ];
        }
        function test_popovers(data) {
            var test = createTest(data.component);

            var mainButton = findChild(test, "mainButton");
            verify(mainButton);
            triggeredSpy.target = mainButton.action;
            keyClick(Qt.Key_T, Qt.ControlModifier);
            triggeredSpy.wait(200);
            mouseClick(mainButton, centerOf(mainButton).x, centerOf(mainButton).y);
            tryCompareFunction(function() { return test.popover != null;}, true, 1000);

            // trigger the action
            triggeredSpy.clear();
            keyClick(Qt.Key_T, Qt.ControlModifier);
            expectFailContinue(data.tag, "Popup is active now");
            triggeredSpy.wait(200);

            var actionItem = findChild(test.popover, "popupButton");
            verify(actionItem);
            triggeredSpy.target = actionItem.action;
            triggeredSpy.clear();
            keyClick(Qt.Key_B, Qt.AltModifier);
            triggeredSpy.wait(200);
        }
    }
}
