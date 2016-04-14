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

                    Button {
                        anchors.centerIn: parent
                        text: "Open dialog"
                        onClicked: {
                            pageStack.popup = PopupUtils.open(dialogComponent);
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
                }
            }
        }
    }

    UbuntuTestCase {
        name: "Dialog"
        when: windowShown
        id: testCase

        SignalSpy {
            id: dialogCloseSpy
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

        function test_close_and_pop_bug1568016() {
            pageStack.push(secondPageComponent);
            var dialog = PopupUtils.open(pageStack.currentPage.dialogComponent);
            dialogCloseSpy.target = dialog.Component;
            dialogCloseSpy.clear();

            PopupUtils.close(dialog);
            pageStack.pop();

            dialogCloseSpy.wait();

            compare(pageStack.depth, 1, "PageStack.pop() failed.");
            compare(pageStack.currentPage, startPage, "Incorrect current page on PageStack.");

            buttonSpy.clear();
            mouseClick(continueButton);
            buttonSpy.wait();
            // FIXME: Compare buttonSpy.count to 1 after bug 1569379 is fixed.
            compare(buttonSpy.count > 0, true, "Could not click button.");
        }
    }
}
