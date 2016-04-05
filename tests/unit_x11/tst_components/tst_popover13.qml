/*
 * Copyright 2014-2015 Canonical Ltd.
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
import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

MainView {
    id: main
    width: units.gu(50)
    height: units.gu(71)

    Rectangle {
        id: rect
        y: main.height / 2
        height: units.gu(10)
        width: height
        MouseArea {
            id: whiteSpace
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
        }

        Button {
            id: pressMe
            anchors.top: parent.top
            text: "Press me"
            onClicked: {
                testCase.resetPositions();
                var popover = PopupUtils.open(popoverComponent, pressMe);
                popoverSpy.target = testCase.findChild(popover, "popover_foreground");
                popoverSpy.clear();
                pressMe.parent.height = units.gu(25)
                pressMe.anchors.top = parent.bottom
            }
        }

        Button {
            id: pushMe
            anchors.bottom: parent.bottom
            text: "Push me"
            onClicked: {
                testCase.resetPositions();
                var popover = PopupUtils.open(popoverComponent, pushMe);
                popoverSpy.target = testCase.findChild(popover, "popover_foreground");
                popoverSpy.clear();
                rect.y = main.height / 10
            }
        }
    }
    Label {
        id: other
        text: "Ignore me"
        anchors.centerIn: parent
    }

    // spy to listen on the popover foreground's hideCompleted() signal
    SignalSpy {
        id: popoverSpy
        signalName: "hideCompleted"
    }
    SignalSpy {
        id: whiteSpy
        signalName: "clicked"
        target: whiteSpace
    }

    Component {
        id: popoverComponent
        Popover {
            objectName: "popover"
            Rectangle {
                width: units.gu(20)
                height: units.gu(20)
                color: "blue"
            }
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "PopoverTests"
        when: windowShown

        function resetPositions() {
            pressMe.parent.height = units.gu(10)
            rect.y = main.height / 2
        }

        function cleanup() {
            resetPositions()
            popoverSpy.target = null;
            popoverSpy.clear();
            waitForRendering(main, 500);
        }

        function test_dismiss_on_click_data() {
            return [
                        {button: Qt.LeftButton},
                        {button: Qt.MiddleButton},
                        {button: Qt.RightButton},
                    ];
        }

        function test_dismiss_on_click(data) {
            mouseClick(pressMe, pressMe.width / 2, pressMe.height / 2);
            waitForRendering(pressMe);
            verify(popoverSpy.target !== null, "The popover did not open");

            // dismiss
            mouseClick(main, 10, 10, data.button);
            popoverSpy.wait();
        }

        function test_dismiss_on_key() {
            mouseClick(pressMe, pressMe.width / 2, pressMe.height / 2);
            waitForRendering(pressMe);
            verify(popoverSpy.target !== null, "The popover did not open");

            keyClick(Qt.Key_Escape);
            popoverSpy.wait();
            // Dismiss via click to avoid false negatives on other cases
            mouseClick(main, 10, 10, Qt.LeftButton);
        }

        function test_popover_consumes_clicks_bug1488540_data() {
            return [
                { tag: 'Left-click', button: Qt.LeftButton },
                { tag: 'Right-click', button: Qt.RightButton },
                { tag: 'Middle-click', button: Qt.MiddleButton },
            ]
        }
        function test_popover_consumes_clicks_bug1488540(data) {
            mouseClick(pressMe, pressMe.width / 2, pressMe.height / 2);
            waitForRendering(pressMe);
            verify(popoverSpy.target !== null, "The popover did not open");
            var popover = popoverSpy.target;
            // Click in the popover, the rectangle doesn't handle clicks
            whiteSpy.clear();
            mouseClick(popover, popover.width / 2, popover.height / 2, data.button);
            // dismiss
            mouseClick(main, 10, 10, Qt.LeftButton);
            popoverSpy.wait();
            // Did the click reach through the popover foreground?
            compare(whiteSpy.count, 0, 'Click passed through popover foreground!');

        }

        function test_popover_follows_pointerTarget_bug1199502_data() {
            return [
                { tag: "Moving pointerTarget", button: pressMe, dir: "down", y: 318 },
                // FIXME: { tag: "Moving parent", button: pushMe, dir: "up", y: 142.8 },
                // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1427557
            ]
        }
        function test_popover_follows_pointerTarget_bug1199502(data) {
            mouseClick(data.button, data.button.width / 2, data.button.height / 2);
            waitForRendering(data.button);
            var dir = popoverSpy.target.direction
            var popoverY = popoverSpy.target.y

            // dismiss
            mouseClick(main, 10, 10, Qt.LeftButton);
            popoverSpy.wait();

            // ensure popover was next to caller
            compare(dir, data.dir, "Popover arrow is wrong")
            compare(popoverY, data.y, "Popover isn't pointing at the caller")
        }
    }
}
