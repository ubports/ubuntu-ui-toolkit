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
import Ubuntu.Components 1.1

Item {
    id: root
    width: 200
    height: 200
    
    // The object under test
    MouseArea {
        id: objectUnderTest
        anchors.fill: parent
        hoverEnabled: true   // required for pure mouseMove events
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        
        onCanceled: {
            console.debug("onCancelled");
        }
        
        onClicked: {
            console.debug("onClicked b:" + mouse.button);
        }
        
        onDoubleClicked: {
            console.debug("onDoubleClicked b:" + mouse.button);
        }
        
        onEntered: {
            console.debug("onEntered");
        }
        
        onExited: {
            console.debug("onExited");
        }
        
        onPositionChanged: {
            console.debug("onPositionChanged x:" + mouseX + " y:" + mouseY);
        }
        
        onPressAndHold: {
            console.debug("onPressAndHold b:" + mouse.button);
        }
        
        onPressed: {
            console.debug("onPressed b:" + mouse.button);
        }
        
        onReleased: {
            console.debug("onReleased b:" + mouse.button);
        }
    }
    

    TestCase {
        name: "ExampleMouse"
        when: windowShown
        
        function init() {
            console.debug(">> init");
            console.debug("<< init");
        }
        
        function cleanup() {
            console.debug(">> cleanup");
            mouseMove(root);
            mouseRelease(root);
            console.debug("<< cleanup");
        }
        
        function initTestCase() {
            console.debug(">> initTestCase");
            console.debug("<< initTestCase");
        }
        
        function cleanupTestCase() {
            console.debug(">> cleanupTestCase");
            console.debug("<< cleanupTestCase");
        }
        
        /*
         * test for a signal existence
         */
        
        // API: function mouseRelease(item, x, y, button, modifiers, delay)
        // API: function mousePress(item, x, y, button, modifiers, delay)
        // API: function mouseDrag(item, x, y, dx, dy, button, modifiers, delay)
        // API: function mouseClick(item, x, y, button, modifiers, delay)
        // API: function mouseDoubleClick(item, x, y, button, modifiers, delay)
        // API: function mouseMove(item, x, y, delay, buttons)
        // API: function mouseWheel(item, x, y, xDelta, yDelta, buttons, modifiers, delay)
       
        function test_mouse_click() {
           mouseClick(objectUnderTest,2,2,Qt.LeftButton,Qt.NoModifiers,100);
           mouseClick(objectUnderTest,10,2,Qt.RightButton);
        }
        
        function test_mouse_doubleclick() {
           mouseDoubleClick(objectUnderTest,10,2,Qt.RightButton);
           mouseDoubleClick(objectUnderTest,2,2,Qt.LeftButton,Qt.NoModifiers,100);
        }

        function test_mouse_move() {
            mouseMove(objectUnderTest,20,20);
        }
        
        function test_mouse_drag() {
            mouseDrag(objectUnderTest,1,1,10,10,Qt.LeftButton);
        }

        
        // end of tests
    }

}
