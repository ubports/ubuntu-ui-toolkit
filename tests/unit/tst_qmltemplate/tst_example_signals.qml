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
    
    // The objects
    Button {
        id: objectUnderTest
        anchors.margins: 20
        anchors.fill: parent
    }

    TestCase {
        name: "ExampleCheckSignals"
        when: windowShown
        
        SignalSpy {
            id: signalSpy
            target: objectUnderTest
        }
        
        function init() {
            console.debug(">> init");
            console.debug("<< init");    
        }
        
        function cleanup() {
            console.debug(">> cleanup");
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
        function test_signals_exists_data() {
	        return [
	                { tag: "clicked", signalName: "clicked", validSignal: true },
          		    { tag: "hovered", signalName: "hovered", validSignal: true } 
          		   ];
        }

        function test_signals_exists(data) {
            signalSpy.signalName = data.signalName;
            compare(signalSpy.signalName,data.signalName,"signalName is set to " + data.signalName);
            compare(signalSpy.valid,data.validSignal,"valid is " + data.validSignal);
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
