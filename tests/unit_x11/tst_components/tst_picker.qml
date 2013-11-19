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
    width: units.gu(20)
    height: units.gu(40)

    Column {
        Picker {
            objectName: "linear"
            id: linearShort
            circular: false
            model: objectModel
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
        Picker {
            objectName: "linear"
            id: linearLong
            circular: false
            model: longerModel
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }

        Picker {
            objectName: "objectModelled"
            id: objectModelled
            model: objectModel
            selectedIndex: 2
        }
        Picker {
            objectName: "simpleModelled"
            id: simpleModelled
            model: emptyModel
            selectedIndex: 2
        }
        Picker {
            objectName: "picker"
            id: picker
        }
    }

    ListModel {
        id: emptyModel
    }

    ListModel {
        id: objectModel
        ListElement {
            label: "line1"
        }
        ListElement {
            label: "line2"
        }
        ListElement {
            label: "line3"
        }
    }

    ListModel {
        id: longerModel
        ListElement {
            label: "line1"
        }
        ListElement {
            label: "line2"
        }
        ListElement {
            label: "line3"
        }
        ListElement {
            label: "line4"
        }
        ListElement {
            label: "line5"
        }
        ListElement {
            label: "line6"
        }
    }

    SignalSpy {
        id: spy
    }

    UbuntuTestCase {
        id: testCase
        name: "PickerAPI"
        when: windowShown

        function test_0_circular() {
            compare(picker.circular, true, "default circular");
        }
        function test_0_model() {
            compare(picker.model, undefined, "default model");
        }
        function test_0_delegate() {
            compare(picker.delegate, null, "default delegate");
        }
        function test_0_highlight() {
            compare(picker.highlight, null, "default highlight");
        }
        function test_0_selectedIndex() {
            compare(picker.selectedIndex, 0, "default selectedIndex");
        }

        function test_1_runtimeModel() {
            picker.model = emptyModel;
            compare(picker.selectedIndex, 0, "selectedIndex gets 0");
        }

        function test_1_selectedIndexForEmptyModel() {
            compare(simpleModelled.selectedIndex, 2, "empty modelled picker selectedIndex is 0");
        }

        function test_1_selectedIndexForObjectModel() {
            compare(objectModelled.selectedIndex, 2, "model containing data, picker.selectedIndex");
        }

        function test_2_updateModel() {
            picker.selectedIndex = 1;
            spy.clear();
            spy.signalName = "onSelectedIndexChanged";
            spy.target = picker;
            picker.model = [];
            tryCompare(spy, "count", 1);
        }

        function test_2_updateModel2() {
            objectModelled.selectedIndex = 1;
            spy.clear();
            spy.signalName = "onSelectedIndexChanged";
            spy.target = objectModelled;
            objectModelled.model = [];
            tryCompare(spy, "count", 1);
        }

        function test_4_clickMovesSelection_Long() {
            spy.clear();
            spy.signalName = "onSelectedIndexChanged";
            spy.target = linearLong;
            mouseClick(linearLong, units.gu(1), units.gu(1));
            tryCompare(spy, "count", 0);
            mouseClick(linearLong, units.gu(1), units.gu(18));
            tryCompare(spy, "count", 1);
        }

        function test_3_clickMovesSelection_Short() {
            spy.clear();
            spy.signalName = "onSelectedIndexChanged";
            spy.target = linearShort;
            mouseClick(linearShort, units.gu(1), units.gu(1));
            tryCompare(spy, "count", 0);
            mouseClick(linearShort, units.gu(1), units.gu(18));
            tryCompare(spy, "count", 1);
        }

        function test_5_clickMovesSelection_Long() {
            spy.clear();
            spy.signalName = "onSelectedIndexChanged";
            linearLong.circular = true;

            spy.target = linearLong;
            mouseClick(linearLong, units.gu(1), units.gu(1));
            tryCompare(spy, "count", 1);
            mouseClick(linearLong, units.gu(1), units.gu(18));
            tryCompare(spy, "count", 2);
        }
    }
}
