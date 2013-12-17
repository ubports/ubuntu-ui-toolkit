/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.Pickers 0.1

Item {
    id: testSuite
    width: units.gu(40)
    height: units.gu(71)

    Flow {
        anchors {
            fill: parent
            // give a margin so we can dismiss panels
            topMargin: units.gu(4)
        }

        Button {
            id: defaultAPI
            text: "modeSet"
            property date buttonDate: new Date()
            property Item panel
            onClicked: panel = PickerPanel.openDatePicker(defaultAPI, "buttonDate")
        }
        Button {
            id: modeSet
            text: "modeSet"
            property string mode
            property date buttonDate: new Date()
            property Item panel
            onClicked: panel = PickerPanel.openDatePicker(modeSet, "buttonDate", mode)
        }
    }

    SignalSpy {
        id: closeSpy
        signalName: "closed"
    }

    UbuntuTestCase {
        name: "PickerPanelAPI"
        when: windowShown

        function initTestCase() {
            waitForRendering(testSuite);
        }

        function test_0_clickOnDefaultAPI() {
            mouseClick(defaultAPI, units.gu(1), units.gu(1));
            verify(defaultAPI.panel !== null, "the picker is not opened");
            verify(defaultAPI.panel.picker !== null, "the DatePicker is not defined");
            compare(defaultAPI.panel.pickerMode, "Years|Months|Days", "the mode from the picker is not the default");
            compare(defaultAPI.panel.date, defaultAPI.buttonDate, "the date from the picker differs from the button's");
            compare(defaultAPI.panel.caller, defaultAPI, "wrong caller");
            compare(defaultAPI.panel.callerProperty, "buttonDate", "wrong callerProperty");
            verify(defaultAPI.panel.hasOwnProperty("closed"), "the object has no closed signal");

            // dismiss
            closeSpy.clear();
            closeSpy.target = defaultAPI.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_1_modeSet_YM() {
            modeSet.mode = "Years|Months" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            compare(picker.children.length, 3, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_1_modeSet_YD() {
            modeSet.mode = "Years|Days" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            expectFailContinue("", "this mode is invalid");
            compare(picker.children.length, 2, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_1_modeSet_HMS() {
            modeSet.mode = "Hours|Minutes|Seconds" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            compare(picker.children.length, 4, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_1_modeSet_HS() {
            modeSet.mode = "Hours|Seconds" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            expectFailContinue("", "this mode is invalid");
            compare(picker.children.length, 2, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        // forced panel tests
        // these should be executed as last ones
        function test_2_clickOnDefaultAPI() {
            // force panel - this is private specific!!!
            var privates = findChild(PickerPanel, "PickerPanel_Internals");
            privates.isPhone = true;

            mouseClick(defaultAPI, units.gu(1), units.gu(1));
            verify(defaultAPI.panel !== null, "the picker is not opened");
            verify(defaultAPI.panel.picker !== null, "the DatePicker is not defined");
            compare(defaultAPI.panel.pickerMode, "Years|Months|Days", "the mode from the picker is not the default");
            compare(defaultAPI.panel.date, defaultAPI.buttonDate, "the date from the picker differs from the button's");
            compare(defaultAPI.panel.caller, defaultAPI, "wrong caller");
            compare(defaultAPI.panel.callerProperty, "buttonDate", "wrong callerProperty");
            verify(defaultAPI.panel.hasOwnProperty("closed"), "the object has no closed signal");

            // dismiss
            closeSpy.clear();
            closeSpy.target = defaultAPI.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_3_modeSet_YM() {
            modeSet.mode = "Years|Months" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            compare(picker.children.length, 3, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_3_modeSet_YD() {
            modeSet.mode = "Years|Days" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            expectFailContinue("", "this mode is invalid");
            compare(picker.children.length, 2, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_3_modeSet_HMS() {
            modeSet.mode = "Hours|Minutes|Seconds" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            compare(picker.children.length, 4, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }

        function test_3_modeSet_HS() {
            modeSet.mode = "Hours|Seconds" ;
            mouseClick(modeSet, units.gu(1), units.gu(1));
            verify(modeSet.panel !== null, "the picker is opened");
            compare(modeSet.panel.date, modeSet.buttonDate, "the date from the picker differs from the button's");
            compare(modeSet.panel.pickerMode, modeSet.mode, "the mode from the picker differs from the button's");
            // check the number of pickers
            var picker = findChild(modeSet.panel.picker, "PickerRow_Positioner");
            expectFailContinue("", "this mode is invalid");
            compare(picker.children.length, 2, "there is not enough pickers in the panel");

            // dismiss
            closeSpy.clear();
            closeSpy.target = modeSet.panel;
            mouseClick(testSuite, units.gu(1), units.gu(1));
            closeSpy.wait();
        }
    }
}
