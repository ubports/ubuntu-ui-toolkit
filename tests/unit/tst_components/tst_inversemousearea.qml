/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 0.1

Item {
    id: testCase
    width: 100; height: 100

    MouseArea {
        id: fullArea
        anchors.fill: parent
        SignalSpy {
            id: masterSpy
            target: parent
        }
    }

    InverseMouseArea {
        id: ima
        x: 5; y: 5
        width: 10; height: 10
        SignalSpy {
            id: signalSpy
            target: parent
        }
    }

    TestCase {
        name: "InverseMouseAreaAPI"

        function test_0_pressed() {
            compare(ima.pressed, false, "InverseMouseArea is not pressed by default");
            try {
                ima.pressed = true;
            } catch (e) {
                //expect failure
            }
            compare(ima.pressed, false, "InverseMouseArea is not pressed by default");
        }

        function test_0_acceptedButtons() {
            compare(ima.acceptedButtons, Qt.LeftButton, "InverseMouseArea accepts LeftButton by default");
            ima.acceptedButtons = Qt.LeftButton | Qt.RightButton;
            compare(ima.acceptedButtons, Qt.LeftButton | Qt.RightButton, "InverseMouseArea acceptButtons is not properly set");
            ima.acceptedButtons = Qt.RightButton;
            compare(ima.acceptedButtons, Qt.RightButton, "InverseMouseArea acceptButtons is not properly set");
            ima.acceptedButtons = Qt.MiddleButton;
            compare(ima.acceptedButtons, Qt.MiddleButton, "InverseMouseArea acceptButtons is not properly set");
            ima.acceptedButtons = Qt.AllButtons;
            compare(ima.acceptedButtons, Qt.AllButtons, "InverseMouseArea acceptButtons is not properly set");
        }

        function test_0_pressedButtons() {
            compare(ima.pressedButtons, Qt.NoButton, "InverseMouseArea has NoButton pressed by default");
            try {
                ima.pressed = Qt.LeftButton;
            } catch (e) {
                //expect failure
            }
            compare(ima.pressedButtons, Qt.NoButton, "InverseMouseArea has NoButton pressed by default");
        }

        function test_0_propagateComposedEvents() {
            compare(ima.propagateComposedEvents, false, "InverseMouseArea does not propagate composed events by default")
            ima.propagateComposedEvents = true;
            compare(ima.propagateComposedEvents, true, "InverseMouseArea does propagates composed events")
        }

        function test_onPressed() {
            ima.acceptedButtons = Qt.LeftButton;
            ima.propagateComposedEvents = false;
            signalSpy.signalName = "pressed";
            masterSpy.signalName = "pressed";
            mousePress(testCase, 1, 1);
            tryCompare(masterSpy,"count",0,"MouseArea pressed was not emitted");
            tryCompare(signalSpy,"count",1,"InverseMouseArea pressed was emitted");

            ima.propagateComposedEvents = false;
            tryCompare(masterSpy,"count",1,"MouseArea pressed was not emitted");
            tryCompare(signalSpy,"count",1,"InverseMouseArea pressed was emitted");
        }

        function test_onClicked() {
            ima.acceptedButtons = Qt.LeftButton;
            ima.propagateComposedEvents = false;
            signalSpy.signalName = "clicked";
            masterSpy.signalName = "clicked";
            mousePress(testCase, 1, 1);
            tryCompare(masterSpy,"count",0,"MouseArea clicked() was not emitted");
            tryCompare(signalSpy,"count",1,"InverseMouseArea clicked() was emitted");

            ima.propagateComposedEvents = true;
            mousePress(testCase, 1, 1);
            tryCompare(masterSpy,"count",1,"MouseArea clicked() was not emitted");
            tryCompare(signalSpy,"count",1,"InverseMouseArea clicked() was emitted");
        }
    }
}

