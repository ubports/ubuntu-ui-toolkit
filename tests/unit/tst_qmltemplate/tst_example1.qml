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

TestCase {
    name: "Example1"
    
    
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
    function test_signal_clicked_exists() {
        signalSpy.signalName = "clicked";
        compare(signalSpy.valid,true,signalSpy.signalName + " signal exists")
    }
    
    /*
     * test for signal emission
     */
    function test_signal_clicked_emission() {
        signalSpy.signalName = "clicked";
        
    }
    
    Button {
        id: objectUnderTest
        SignalSpy {
            id: signalSpy
            target: parent
        }
    }
    
    Label {
        id: testObject1
    }
}

