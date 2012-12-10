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
    id: root
    width: 200
    height: 200
    
    
    
    // The objects
    Button {
        id: objectUnderTest
        anchors.margins: 20
        anchors.fill: parent
    }

    Label {
        id: testObject1
    }

    

    TestCase {
        name: "Example1"
        when: windowShown
        
       SignalSpy {
        id: signalSpy
        target: objectUnderTest
       }
        
        function init() {
            console.debug("init is called before each test");
        }
        
        function cleanup() {
            console.debug("cleanup is called after each test");
        }
        
        function initTestCase() {
            console.debug("initTestCase is called once before running any tests");
        }
        
        function cleanupTestCase() {
            console.debug("cleanupTestCase is called once after all tests");
        }
        
        /* 
         * the data for the test_text(data)
         */
        function test_text_data() {
            return [
                    { tag: "A normal text", text: "Hello World!", expectFail: false },
                    { tag: "Unable to set object* to text", text: testObject1, expectFail: true }
                   ];
        }
        
        /*
         * a data driven test function
         */
        function test_text(data) {
            // check if the expectFail flag is up in data
            if (data.expectFail) {
                expectFail("",data.tag);
            }
            
            // try to set the data
            try {
                objectUnderTest.text = data.text;
            } catch (e) {
                // catched an exception, check if we are expecting a failure, if we are then lets ignore.
                if (!data.expectFail) { console.error(e); } else { console.debug(e); }
            }
            
            // do the comparing
            compare(objectUnderTest.text,data.text,"Was able to set " + data.text + " as text.");
        }
        
        /*
         * test for a signal existence
         */
        function test_signals_exists_data() {
	    return [{ tag: "clicked", signalName: "clicked", validSignal: true },
      		{ tag: "hovered", signalName: "hovered", validSignal: true } ];
        }

        function test_signals_exists(data) {
            signalSpy.signalName = data.signalName;
            compare(signalSpy.signalName,data.signalName,"signalName is set to " + data.signalName);
            compare(signalSpy.valid,data.validSignal,signalSpy.signalName + " signal exists")
        }
        
        function test_signal_emission_clicked() {
            signalSpy.signalName = "clicked";
            mouseMove(objectUnderTest,10,10);
            mouseClick(objectUnderTest,10,10);
            tryCompare(signalSpy,"count",1,"clicked was emitted");
        }
        
        // end of tests
    }

}
