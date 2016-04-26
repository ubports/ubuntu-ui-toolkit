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
import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0

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
                var popover = testCase.openPopup(popoverComponent, pressMe);
                pressMe.parent.height = units.gu(25)
                pressMe.anchors.top = parent.bottom
            }
        }

        Button {
            id: pushMe
            anchors.bottom: parent.bottom
            text: "Push me"
            onClicked: {
                var popover = testCase.openPopup(popoverComponent, pushMe);
                rect.y = main.height / 10
            }
        }
    }
    Label {
        id: other
        text: "Ignore me"
        anchors.centerIn: parent
    }

    SignalSpy {
        id: popoverShownSpy
        signalName: "showCompleted"
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

        function openPopup(popupComponent, caller) {
            resetPositions();
            var popover = PopupUtils.open(popoverComponent, caller);
            var foreground = findChild(popover, "popover_foreground");
            popoverSpy.target = foreground;
            popoverSpy.clear();
            popoverShownSpy.target = foreground;
            popoverShownSpy.clear()
        }

        function cleanup() {
            // Dismiss via click to avoid false negatives on other cases
            if (popoverSpy.target) {
                mouseClick(main, 10, 10, Qt.LeftButton);
                popoverSpy.wait();
                popoverSpy.target = null;
                popoverShownSpy.target = null;
            }

            resetPositions()
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
                { tag: "Moving pointerTarget", button: pressMe, otherButton: pushMe, dir: "down" },
                // FIXME: { tag: "Moving parent", button: pushMe, otherButton: pressMe, dir: "up" },
                // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1427557
            ]
        }
        function test_popover_follows_pointerTarget_bug1199502(data) {
            mouseClick(data.button, data.button.width / 2, data.button.height / 2);
            popoverShownSpy.wait();
            var popover = popoverSpy.target;

            // ensure popover was next to caller
            compare(popover.direction, data.dir, "Popover arrow is wrong");
            // Compare button in context of the root coordinate space
            var buttonY = main.mapFromItem(data.button, data.button.x, data.button.y).y
            if (data.dir == 'down') {
                verify((popover.y + popover.height) < buttonY, "Popover is above the caller (%1 + %2 < %3)".arg(popover.y).arg(popover.height).arg(buttonY));
                verify((popover.y + popover.height) > data.otherButton.y, "Popover is close to the caller");
            } else {
                verify((popover.y) > (buttonY + data.button.height), "Popover is below the caller");
            }
        }
    }
}
