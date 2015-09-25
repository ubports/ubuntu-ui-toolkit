/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Pickers 1.3

Item {
    id: testSuite
    width: units.gu(40)
    height: units.gu(71)

    Component {
        id: linearShort
        Picker {
            objectName: "linear"
            circular: false
            model: objectModel
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
    }
    Component {
        id: linearLong
        Picker {
            objectName: "linear"
            circular: false
            model: longerModel
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
    }
    Component {
        id: objectModelled
        Picker {
            objectName: "objectModelled"
            model: objectModel
            selectedIndex: 2
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
    }
    Component {
        id: simpleModelled
        Picker {
            objectName: "simpleModelled"
            model: emptyModel
            selectedIndex: 2
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
    }
    Component {
        id: linearDynPicker
        Picker {
            objectName: "linearDynPicker"
            model: dynamicModel
            circular: false
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
    }
    Component {
        id: circularDynPicker
        Picker {
            objectName: "circularDynPicker"
            model: dynamicModel
            circular: true
            delegate: PickerDelegate {
                Label { text: modelData ? modelData : "" }
            }
        }
    }
    Component {
        id: defaultPicker
        Picker {
        }
    }

    Loader {
        id: pickerLoader
        asynchronous: false
    }

    ListModel {
        id: emptyModel
    }

    ListModel {
        id: dynamicModel
        Component.onCompleted: reset()
        function reset() {
            clear();
            for (var i = 0; i < 100; i++) {
                append({"label": "line" + i});
            }
        }
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
        signalName: "onSelectedIndexChanged"
    }

    UbuntuTestCase {
        id: testCase
        name: "Picker13API"
        when: windowShown

        function testPicker(component) {
            pickerLoader.sourceComponent = component;
            tryCompareFunction(function(){return pickerLoader.status}, Loader.Ready);
            waitForRendering(pickerLoader.item);
            tryCompareFunction(function(){return pickerLoader.item.moving}, false);
            return pickerLoader.item;
        }

        function waitPickerScrolling(picker) {
            tryCompareFunction(function(){return picker.moving}, false);
        }

        function getPickerList(picker) {
            return findChild(picker, picker.circular ? "Picker_WrapAround" : "Picker_Linear");
        }

        function cleanup() {
            pickerLoader.sourceComponent = null;
            dynamicModel.reset();
            spy.clear();
            spy.target = null;
        }

        function initTestCase() {
            var picker = testPicker(defaultPicker);
            compare(picker.circular, true, "default circular");
            compare(picker.model, undefined, "default model");
            compare(picker.delegate, null, "default delegate");
            compare(picker.selectedIndex, 0, "default selectedIndex");
            compare(picker.itemHeight, units.gu(4), "default itemHeight");
            compare(picker.__styleInstance.highlightThickness, units.gu(5), "default highlight thickness");
        }

        function test_selectedIndex_data() {
            return [
                {tag: "Runtime model", picker: defaultPicker, model: emptyModel, expectedIndex: 0},
                {tag: "Empty modelled", picker: simpleModelled, expectedIndex: 0},
                {tag: "Object modelled", picker: objectModelled, expectedIndex: 2},
            ];
        }
        function test_selectedIndex(data) {
            var picker = testPicker(data.picker);
            if (data.nmodel) {
                picker.model = data.model;
            }
            compare(picker.selectedIndex, data.expectedIndex, "unexpected selectedIndex");
        }

        function test_updateModel_data() {
            return [
                {tag: "Empty picker", picker: defaultPicker},
                {tag: "Object modelled picker", picker: objectModelled},
            ];
        }
        function test_updateModel(data) {
            var picker = testPicker(data.picker);
            picker.selectedIndex = 1;
            spy.target = picker;
            picker.model = [];
            spy.wait();
        }

        function test_clickMovesSelection_data() {
            return [
                {tag: "Linear long", component: linearLong, circular: false, firstClick: 0, secondClick: 1},
                {tag: "Circular long", component: linearLong, circular: true, firstClick: 1, secondClick: 2},
                {tag: "Linear short", component: linearShort, circular: false, firstClick: 0, secondClick: 1},
                {tag: "Circular short", component: linearShort, circular: true, firstClick: 0, secondClick: 1},
            ];
        }
        function test_clickMovesSelection(data) {
            var picker = testPicker(data.component);
            spy.target = picker;
            picker.circular = data.circular;

            mouseClick(picker, units.gu(1), units.gu(1));
            tryCompare(spy, "count", data.firstClick);
            mouseClick(picker, units.gu(1), units.gu(18));
            tryCompare(spy, "count", data.secondClick);
        }

        function test_pickerCircularChange() {
            var picker = testPicker(defaultPicker);
            var expectedList = picker.circular ? "Picker_WrapAround" : "Picker_Linear";
            verify(findChild(picker, expectedList) !== undefined, "Picker must use " + expectedList);

            picker.circular = !picker.circular;
            expectedList = picker.circular ? "Picker_WrapAround" : "Picker_Linear";
            verify(findChild(picker, expectedList) !== undefined, "circular changed, Picker must use " + expectedList);
        }

        function test_modelCropping_data() {
            return [
                {tag: "Linear, selected @50, remove from 40", component: linearDynPicker, selected: 50, remove: 40, removeCount: dynamicModel.count - 40, count: 40, expected: 39 },
                {tag: "Linear, selected @50, remove 50", component: linearDynPicker, selected: 50, remove: 50, removeCount: 1, count: dynamicModel.count - 1, expected: 49 },
                {tag: "Linear, selected @50, remove 10 from 45", component: linearDynPicker, selected: 50, remove: 45, removeCount: 10, count: dynamicModel.count - 10, expected: 44 },
                {tag: "Linear, selected @50, remove 10 from 51", component: linearDynPicker, selected: 50, remove: 51, removeCount: 10, count: dynamicModel.count - 10, expected: 50 },

                {tag: "Circular, selected @50, remove from 40", component: circularDynPicker, selected: 50, remove: 40, removeCount: dynamicModel.count - 40, count: 40, expected: 39 },
                {tag: "Circular, selected @50, remove 50", component: circularDynPicker, selected: 50, remove: 50, removeCount: 1, count: dynamicModel.count - 1, expected: 49 },
                {tag: "Circular, selected @50, remove 10 from 45", component: circularDynPicker, selected: 50, remove: 45, removeCount: 10, count: dynamicModel.count - 10, expected: 44 },
                {tag: "Circular, selected @50, remove 10 from 51", component: circularDynPicker, selected: 50, remove: 51, removeCount: 10, count: dynamicModel.count - 10, expected: 50 },
            ]
        }

        function test_modelCropping(data) {
            var picker = testPicker(data.component);
            picker.selectedIndex = data.selected;
            waitPickerScrolling(picker);
            var list = getPickerList(picker);

            picker.model.remove(data.remove, data.removeCount);
            waitPickerScrolling(picker);
            compare(list.count, data.count, "bad removal from list");
            compare(picker.selectedIndex, data.expected, "bad index of " + data.tag);
        }

        function test_modelReset_data() {
            return [
                {tag: "Linear", picker: linearDynPicker},
                {tag: "Circular", picker: circularDynPicker},
            ];
        }
        function test_modelReset(data) {
            var picker = testPicker(data.picker);
            var list = getPickerList(picker);
            picker.model.reset();
            waitPickerScrolling(picker);
            compare(list.currentIndex, 0, "picker's itemList selection not reset");
            compare(picker.selectedIndex, 0, "picker's selection not reset");
        }

        function test_modelClear_data() {
            return [
                {tag: "Linear", picker: linearDynPicker},
                {tag: "Circular", picker: circularDynPicker},
            ];
        }
        function test_modelClear(data) {
            var picker = testPicker(data.picker);
            var list = getPickerList(picker);
            picker.model.clear();
            if (data.tag == "Linear") {
                // picker's ListView needs some events to be processed before it gets the final value for currentIndex
                waitForRendering(list, 500);
            }
            waitPickerScrolling(picker);
            if (data.tag == "Circular") {
                expectFailContinue(data.tag, "PathView issue: https://bugreports.qt-project.org/browse/QTBUG-35400");
            }
            compare(list.currentIndex, -1, "picker's itemList selection not reset");
            if (data.tag == "Circular") {
                expectFailContinue(data.tag, "PathView issue: https://bugreports.qt-project.org/browse/QTBUG-35400");
            }
            compare(picker.selectedIndex, -1, "picker's selection not reset");
        }

        function test_itemHeight_data() {
            return [
                {tag: "Linear", picker: linearDynPicker, itemHeight: units.gu(6)},
                {tag: "Circular", picker: circularDynPicker, itemHeight: units.gu(6)},
            ];
        }
        function test_itemHeight(data) {
            var picker = testPicker(data.picker);
            picker.itemHeight = data.itemHeight;
            compare(picker.__styleInstance.highlightThickness, picker.itemHeight + units.gu(1));
        }
    }
}
