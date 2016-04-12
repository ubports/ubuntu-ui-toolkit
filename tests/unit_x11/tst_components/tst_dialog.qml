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
    objectName: "root"
//Component.onCompleted: print("root = "+root)

    MainView {
        anchors.fill: parent
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
                        print("clickily")
                        pageStack.push(secondPageComponent)
                    }
                }

                Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    height: units.gu(10)
                    color: mouseArea.pressed ? "red" : "blue"
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                    }
                }

            }

//            Timer {
//                id: timer
//                interval: 1000
//                repeat: false
//                onTriggered: {
//                    print("closing..")
//                    PopupUtils.close(dialog);
//                    print("popping..")
//                    pageStack.pop();
//                    print("done.")
//                }
//                property Dialog dialog
//            }

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
//                            timer.dialog = popup;
//                            timer.start();
                        }
                    }

//                    Timer {
//                        id: timer
//                        interval: 1000
//                        repeat: false
//                        onTriggered: {
//                            print("closing..")
//                            PopupUtils.close(secondPage.popup);
//                            print("popping..")
//                            pageStack.pop();
//                            print("done.")
//                        }
//                    }

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
                                    print("closing..")
                                    PopupUtils.close(pageStack.popup);
                                    print("popping..")
                                    pageStack.pop();
                                    print("done.")
                                }
                            }
                            Component.onDestruction: print("destroying dialog")
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
            id: buttonSpy
            signalName: 'clicked'
            target: continueButton
            onCountChanged: print("new count = "+count)
        }

        function initTestCase() {
        }

        function cleanup() {
            pageStack.clear();
            pageStack.push(startPage);
        }

        function test_close_and_pop_bug1568016() {
            pageStack.push(secondPageComponent);
            var popup = PopupUtils.open(pageStack.currentPage.dialogComponent);
            PopupUtils.close(popup);
            pageStack.pop();

            compare(pageStack.depth, 1, "PageStack.pop() failed.");
            compare(pageStack.currentPage, startPage, "Incorrect current page on PageStack.");

            buttonSpy.clear();
            print("spy count after clearing = "+buttonSpy.count)
            mouseClick(continueButton)
            // FIXME: Compare buttonSpy.count to 1 after bug 1569379 is fixed.
            compare(buttonSpy.count > 0, true, "Could not click button.");
        }
    }

}
