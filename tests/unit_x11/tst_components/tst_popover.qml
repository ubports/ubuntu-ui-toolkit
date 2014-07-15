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
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Button {
        id: caller
        anchors.centerIn: parent
        text: "Press me"
        onClicked: {
            var popover = PopupUtils.open(popoverComponent, caller);
            popoverSpy.target = testCase.findChild(popover, "popover_foreground");
            popoverSpy.clear();
        }
    }

    // spy to listen on the popover foreground's hideCompleted() signal
    SignalSpy {
        id: popoverSpy
        signalName: "hideCompleted"
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

        function cleanup() {
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
            mouseClick(caller, caller.width / 2, caller.height / 2);
            waitForRendering(caller);
            verify(popoverSpy.target !== null, "The popover did not open");
            // dismiss
            mouseClick(main, 10, 10, data.button);
            popoverSpy.wait();
        }
    }
}
