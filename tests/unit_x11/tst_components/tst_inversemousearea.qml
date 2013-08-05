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

    Item {
        id: testSensingArea
        anchors{
            fill: parent
            topMargin: 5
        }
    }

    MouseArea {
        id: fullArea
        anchors.fill: parent
    }

    InverseMouseArea {
        id: ima
        x: 10; y: 10
        width: 10; height: 10
        property bool acceptPressEvent: true
        property bool acceptClickEvent: true
        onPressed: mouse.accepted = acceptPressEvent
        onClicked: mouse.accepted = acceptClickEvent
    }

    TestCase {
        name: "InverseMouseAreaAPI"
        when: windowShown

        SignalSpy {
            id: masterSpy
            target: fullArea
        }

        SignalSpy {
            id: signalSpy
            target: ima
        }

        function test_0_pressed() {
            compare(ima.pressed, false, "InverseMouseArea is not pressed by default");
            try {
                ima.pressed = true;
                compare(true, false, "This should not be reached");
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
                compare(true, false, "This should not be reached");
            } catch (e) {
                //expect failure
            }
            compare(ima.pressedButtons, Qt.NoButton, "InverseMouseArea has NoButton pressed by default");
        }

        function test_0_propagateComposedEvents() {
            compare(ima.propagateComposedEvents, false, "InverseMouseArea does not propagate composed events by default");
            ima.propagateComposedEvents = true;
            compare(ima.propagateComposedEvents, true, "InverseMouseArea does propagates composed events");
        }

        function test_0_sensingArea() {
            compare(ima.sensingArea, QuickUtils.rootItem(ima), "InverseMouseArea senses the root item area");
            ima.sensingArea = testSensingArea
            compare(ima.sensingArea, testSensingArea, "InverseMouseArea sensing area set to testSensingArea");
        }

        function test_signals_data() {
            return [
                {signal: "onPressed", propagate: false, sensing: null, item: testCase, x: 1, y: 1, master: 0, inverse: 1, acceptClickEvent: false},
                {signal: "onPressed", propagate: true, sensing: null, item: testCase, x: 1, y: 1, master: 1, inverse: 1, acceptPressEvent: false},
                {signal: "onPressed", propagate: false, sensing: testSensingArea, item: testCase, x: 1, y: 1, master: 1, inverse: 0},
                {signal: "onPressed", propagate: true, sensing: testSensingArea, item: testCase, x: 1, y: 1, master: 1, inverse: 0},

                {signal: "onReleased", propagate: false, sensing: null, item: testCase, x: 1, y: 1, master: 0, inverse: 1, acceptClickEvent: false},
                {signal: "onReleased", propagate: true, sensing: null, item: testCase, x: 1, y: 1, master: 1, inverse: 0, acceptPressEvent: false},
                {signal: "onReleased", propagate: false, sensing: testSensingArea, item: testCase, x: 1, y: 1, master: 1, inverse: 0},
                {signal: "onReleased", propagate: true, sensing: testSensingArea, item: testCase, x: 1, y: 1, master: 1, inverse: 0},

                {signal: "onClicked", propagate: false, sensing: null, item: testCase, x: 1, y: 1, master: 0, inverse: 1},
                {signal: "onClicked", propagate: true, sensing: null, item: testCase, x: 1, y: 1, master: 1, inverse: 1, acceptClickEvent: false},
                {signal: "onClicked", propagate: false, sensing: testSensingArea, item: testCase, x: 1, y: 1, master: 1, inverse: 0},
                {signal: "onClicked", propagate: true, sensing: testSensingArea, item: testCase, x: 1, y: 1, master: 1, inverse: 0},
            ];
        }

        function test_signals(data) {
            masterSpy.clear();
            signalSpy.clear();
            masterSpy.signalName = data.signal;
            signalSpy.signalName = data.signal;
            if (data.acceptPressEvent !== undefined) {
                ima.acceptPressEvent = data.acceptPressEvent;
            } else {
                ima.acceptPressEvent = true;
            }
            if (data.acceptClickEvent !== undefined) {
                ima.acceptClickEvent = data.acceptClickEvent;
            } else {
                ima.acceptClickEvent = true;
            }

            ima.acceptedButtons = Qt.LeftButton;
            ima.propagateComposedEvents = data.propagate;
            ima.sensingArea = data.sensing;

            mouseClick(testCase, data.x, data.y);
            tryCompare(masterSpy, "count", data.master, 100);
            tryCompare(signalSpy, "count", data.inverse, 100);
        }
    }
}

