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
    
    focus: true // required for Keys
    
    property string text : ""
    
    Keys.onPressed: {
        console.debug("onPressed: " + event.key);
        text += event.text;
    }
    Keys.onReleased: {
        console.debug("onReleased: " + event.key);
        
    }


    TestCase {
        name: "ExampleKeys"
        when: windowShown
        
        function init() {
            console.debug(">> init is called before each test");

            console.debug("<< init is called before each test");
        }
        
        function cleanup() {
            console.debug(">> cleanup");
            root.text = "";
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
        
        // API: function keyPress(key, modifiers, delay)
        // API: function keyRelease(key, modifiers, delay)
        // API: function keyClick(key, modifiers, delay)
       
        function test_keyClick() {
           keyClick(Qt.Key_H);
           keyClick(Qt.Key_E);
           keyClick(Qt.Key_L);
           keyClick(Qt.Key_L);
           keyClick(Qt.Key_O);
           compare(root.text,"hello","Text was inserted by keyboard events");
        }
        
        function test_keyPress_and_keyRelease() {
           keyPress(Qt.Key_E);
           keyRelease(Qt.Key_E);
           compare(root.text,"e","Text was inserted by keyboard events");
        }
        
        
        // end of tests
    }

}
