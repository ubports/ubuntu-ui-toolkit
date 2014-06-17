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
import Ubuntu.Test 1.0

Rectangle {
 id: root
 width: 800
 height: 600

 Column {
     anchors.fill: parent
     MultiPointTouchArea {
         id: touchArea
         width: parent.width
         height: 100
         touchPoints: TouchPoint {
             id: point
         }
     }

     MouseArea {
        id: mouseArea
        objectName: "myMouseArea"
        width: parent.width
        height: 200
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
        property int testX : 0
        property int testY : 0
        property int steps : 0

        onPositionChanged: {
           testX = mouseX;
           testY = mouseY;
           steps++;
        }
     }
     Flickable {
         id: flicker
         width: parent.width
         height: 400
         contentWidth: rect.width
         contentHeight: rect.height
         clip: true
         Rectangle {
             id: rect
             color: "blue"
             width: 1000
             height: 1000
         }
     }
 }
 
 UbuntuTestCase {
    name: "TestTheUbuntuTestCase"
    when: windowShown

    function initTestCase() {
        TestExtras.registerTouchDevice();
    }

    function init() {
        mouseArea.steps = 0;
    }
    function cleanup() {
        movementSpy.clear();
        longPressSpy.clear();
        touchPressSpy.clear();
        touchReleaseSpy.clear();
        touchUpdateSpy.clear();
    }

    function test_mouseMoveSlowly() {
       mouseMoveSlowly(mouseArea,0,0,800,200,10,100);
       compare(mouseArea.testX,800);
       compare(mouseArea.testY,200);
       compare(mouseArea.steps,10);
    }

    function test_findChild() {
        var child = findChild(root,"myMouseArea");
        compare(child!==null,true, "When a child is found, it is returned");
        compare(child.objectName,"myMouseArea");

        child = findChild(root,"NoSuchChildHere");
        compare(child===null,true,"When there is no child, function should return null");
    }

    SignalSpy {
        id: longPressSpy
        target: mouseArea
        signalName: "onPressAndHold"
    }

    function test_longPress_left() {
        longPressSpy.clear();
        mouseLongPress(mouseArea, mouseArea.width / 2, mouseArea.height / 2);
        longPressSpy.wait();
        // cleanup
        mouseRelease(mouseArea, mouseArea.width / 2, mouseArea.height / 2);
    }

    function test_longPress_right() {
        longPressSpy.clear();
        mouseLongPress(mouseArea, mouseArea.width / 2, mouseArea.height / 2, Qt.RightButton);
        longPressSpy.wait();
        // cleanup
        mouseRelease(mouseArea, mouseArea.width / 2, mouseArea.height / 2, Qt.RightButton);
    }

    function test_longPress_middle() {
        longPressSpy.clear();
        mouseLongPress(mouseArea, mouseArea.width / 2, mouseArea.height / 2, Qt.MiddleButton);
        longPressSpy.wait();
        // cleanup
        mouseRelease(mouseArea, mouseArea.width / 2, mouseArea.height / 2, Qt.MiddleButton);
    }

    SignalSpy {
        id: movementSpy
        target: flicker
        signalName: "onMovementEnded"
    }

    function test_flick_default() {
        flick(flicker, 0, 0, flicker.width, flicker.height);
        movementSpy.wait();
    }
    function test_flick_long() {
        flick(flicker, 0, 0, flicker.width, flicker.height, -1, 10);
        movementSpy.wait();
    }
    function test_flick_short() {
        flick(flicker, 0, 0, flicker.width, flicker.height, -1, 1);
        movementSpy.wait();
    }
    function test_flick_pressTimeout() {
        flick(flicker, 0, 0, flicker.width, flicker.height, 400);
        movementSpy.wait();
    }
    function test_flick_pressTimeout_short() {
        flick(flicker, flicker.width, flicker.height, -flicker.width, -flicker.height, 400, 1);
        movementSpy.wait();
    }
    function test_flick_pressTimeout_long() {
        flick(flicker, flicker.width, flicker.height, -flicker.width, -flicker.height, 400, 100);
        movementSpy.wait();
    }

    SignalSpy {
        id: touchPressSpy
        signalName: "onPressed"
        target: touchArea
    }
    SignalSpy {
        id: touchReleaseSpy
        signalName: "onReleased"
        target: touchArea
    }
    SignalSpy {
        id: touchUpdateSpy
        signalName: "onUpdated"
        target: touchArea
    }

    function test_touchPress() {
        TestExtras.touchPress(0, touchArea, Qt.point(10, 10));
        touchPressSpy.wait();
    }
    function test_touchRelease() {
        TestExtras.touchRelease(0, touchArea, Qt.point(10, 10));
        touchReleaseSpy.wait();
    }
    function test_touchClick() {
        TestExtras.touchClick(0, touchArea, Qt.point(10, 10));
        touchReleaseSpy.wait();
        compare(touchPressSpy.count, 1, "Not pressed?");
        compare(touchReleaseSpy.count, 1, "Not released?");
    }
    function test_touchDoubleClick() {
        TestExtras.touchDoubleClick(0, touchArea, Qt.point(10, 10));
        compare(touchPressSpy.count, 2, "Not pressed twice?");
        compare(touchReleaseSpy.count, 2, "Not released twice?");
    }
    function test_touchMove() {
        TestExtras.touchPress(0, touchArea, Qt.point(0, 0));
        TestExtras.touchMove(0, touchArea, Qt.point(10, 10));
        touchUpdateSpy.wait();
        TestExtras.touchRelease(0, touchArea, Qt.point(10, 10));
    }
    function test_touchDrag_default_steps() {
        TestExtras.touchDrag(0, touchArea, Qt.point(0, 0), Qt.point(10, 10));
        compare(touchPressSpy.count, 1, "Not pressed?");
        compare(touchReleaseSpy.count, 1, "Not released?");
        compare(touchUpdateSpy.count, 5, "Not moved?");
    }
    function test_touchDrag_10_steps() {
        TestExtras.touchDrag(0, touchArea, Qt.point(0, 0), Qt.point(100, 100), 10);
        compare(touchPressSpy.count, 1, "Not pressed?");
        compare(touchReleaseSpy.count, 1, "Not released?");
        compare(touchUpdateSpy.count, 10, "Not moved?");
    }
 }
}
