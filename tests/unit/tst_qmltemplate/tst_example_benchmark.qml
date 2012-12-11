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
    name: "ExampleBenchmarkObject"
    when: windowShown

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
     * the data for the benchmark_create_component(data)
     */
    function benchmark_create_component_data() {
        return [
                { tag: "Button", fileName: "../../../modules/Ubuntu/Components/Button.qml" }
               ];
    }
    
    /*
     * a data driven test function
     */
    function benchmark_create_component(data) {
        var component = Qt.createComponent(data.fileName)
        var obj = component.createObject(top)
        obj.destroy()
        component.destroy()
    }
}
