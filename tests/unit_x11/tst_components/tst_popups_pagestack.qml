/*
 * Copyright 2016 Canonical Ltd.
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

import QtTest 1.0
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Test 1.3

Item {
    id: root
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        anchors.fill: parent
        id: mainview
        objectName: "mainview"

        PageStack {
            id: pageStack
            objectName: "stack"
            property var popup

            Component.onCompleted: push(startPage)

            Page {
                id: startPage
                objectName: "startpage"
                visible: false
                header: PageHeader {
                    title: "Start page"
                    trailingActionBar.actions: [
                        Action {
                            text: "Settings"
                            iconName: "settings"
                        }
                    ]
                }
                Button {
                    id: continueButton
                    anchors.centerIn: parent
                    text: "Continue"
                    onClicked: {
                        pageStack.push(secondPageComponent)
                    }
                }
            }

            Component {
                id: secondPageComponent
                Page {
                    id: secondPage
                    objectName: "secondpage"
                    header: PageHeader {
                        title: "2. Page"
                    }

                    Column {
                        anchors.centerIn: parent

                        Button {
                            text: "Open dialog"
                            onClicked: {
                                pageStack.popup = PopupUtils.open(dialogComponent);
                            }
                        }
                        Button {
                            text: "Open popover"
                            onClicked: {
                                pageStack.popup = PopupUtils.open(popoverComponent);
                            }
                        }
                    }

                    property alias dialogComponent: dialogComponent
                    Component {
                        id: dialogComponent
                        Dialog {
                            id: dialog
                            objectName: "dialog"
                            title: "Dialog"
                            Button {
                                text: "Close and pop page"
                                onClicked: {
                                    PopupUtils.close(pageStack.popup);
                                    pageStack.pop();
                                }
                            }
                        }
                    }
                    property alias popoverComponent: popoverComponent
                    Component {
                        id: popoverComponent
                        Popover {
                            id: popover
                            objectName: "popover"
                            Rectangle {
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }
                                height: units.gu(35)
                                color: UbuntuColors.ash

                                Button {
                                    anchors.centerIn: parent
                                    text: "Close and pop page"
                                    onClicked: {
                                        PopupUtils.close(pageStack.popup);
                                        pageStack.pop();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    UbuntuTestCase {
        name: "Dialog"
        when: windowShown
        id: testCase

        SignalSpy {
            id: popupCloseSpy
            signalName: "onDestruction"
        }

        SignalSpy {
            id: buttonSpy
            signalName: 'clicked'
            target: continueButton
        }

        function cleanup() {
            pageStack.clear();
            pageStack.push(startPage);
        }

        function test_close_and_pop_bug1568016_data() {
            return [
                        {   tag: "Dialog component"     },
                        {   tag: "Popover component"    }
                    ];
        }

        function test_close_and_pop_bug1568016(data) {
            pageStack.push(secondPageComponent);
            waitForHeaderAnimation(mainview); // wait for the push() to finish

            var popup;
            if (data.tag === "Dialog component") {
                popup = PopupUtils.open(pageStack.currentPage.dialogComponent);
            } else { // Popover component
                popup = PopupUtils.open(pageStack.currentPage.popoverComponent);
            }

            popupCloseSpy.target = popup.Component;
            popupCloseSpy.clear();

            PopupUtils.close(popup);
            pageStack.pop();

            popupCloseSpy.wait();
            // Introduce a short delay to wait for the pagestack to finish.
            //  Without this, the test becomes flaky.
            waitForHeaderAnimation(mainview);

            compare(pageStack.depth, 1, "PageStack.pop() failed.");
            compare(pageStack.currentPage, startPage, "Incorrect current page on PageStack.");

            buttonSpy.clear();
            mouseClick(continueButton, continueButton.width/2, continueButton.height/2);
            buttonSpy.wait();
        }
    }
}
