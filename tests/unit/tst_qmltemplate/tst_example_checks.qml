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
    
    property string testString : "i am set"
    property string anotherTestString : ""
    
    TestCase {
        name: "ExampleChecks"
        when: windowShown

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


        // API: function verify(cond, msg)
        // API: function fail(msg)
        // API: function compare(actual, expected, msg)
        // API: function fuzzyCompare(actual, expected, delta, msg)
        // API: function grabImage(item)
        // API: function tryCompare(obj, prop, value, timeout)
        // API: function skip(msg)
        // API: function expectFail(tag, msg)
        // API: function expectFailContinue(tag, msg)
        // API: function warn(msg)
        // API: function wait(ms)
        // API: function waitForRendering(item, timeout)
        // API: function sleep(ms)
       
       /* When you find a bug and you write a test for it,
          use expectFail or expectFailContinue, as then you will notice
          when it has been fixed as XFAIL will turn to XPASS */
          
          // You should NOT use skip as then the bug might be forgotten.
          
        function test_expectFailContinue() {    
            expectFailContinue("","The next check will fail, but we WILL continue further");
            compare(root.testString,root.anotherTestString,"The strings does not match");
            
            verify(root.testString==="i am set","this check gets called even tho we failed earlier");
        }
        
        function test_expectFail() {
            expectFail("","The next one will fail, but we _WILL NOT_ continue any further")    
            compare(root.testString,root.anotherTestString,"The strings does not match");
            
            console.debug("you wont get this far");
            root.testString = "";
            
            compare(root.testString,root.anotherTestString,"The strings does match");
        }
        
    }
}
