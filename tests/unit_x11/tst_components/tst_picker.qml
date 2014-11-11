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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 1.0

Item {
    id: testSuite
    width: units.gu(40)
    height: units.gu(71)

    Flow {
        anchors.fill: parent
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
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
        Picker {
            objectName: "simpleModelled"
            id: simpleModelled
            model: emptyModel
            selectedIndex: 2
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
        Picker {
            objectName: "picker"
            id: picker
        }
        Picker {
            id: linearDynPicker
            objectName: "linearDynPicker"
            model: dynamicModel
            circular: false
            delegate: PickerDelegate {
                Label {text: modelData}
            }
        }
        Picker {
            id: circularDynPicker
            objectName: "circularDynPicker"
            model: dynamicModel
            circular: true
            delegate: PickerDelegate {
                Label { text: modelData ? modelData : "" }
            }
        }
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
    }

    UbuntuTestCase {
        id: testCase
        name: "PickerAPI"
        when: windowShown

        function waitPickerScrolling() {
            wait(600);
        }

        function getPickerList(item, linear) {
            return findChild(item, (linear) ? "Picker_Linear" : "Picker_WrapAround");
        }

        function test_0_circular() {
            compare(picker.circular, true, "default circular");
        }
        function test_0_model() {
            compare(picker.model, undefined, "default model");
        }
        function test_0_delegate() {
            compare(picker.delegate, null, "default delegate");
        }
        function test_0_selectedIndex() {
            compare(picker.selectedIndex, 0, "default selectedIndex");
        }

        function test_1_runtimeModel() {
            picker.model = emptyModel;
            compare(picker.selectedIndex, 0, "selectedIndex gets 0");
        }

        function test_1_selectedIndexForEmptyModel() {
            compare(simpleModelled.selectedIndex, 0, "empty modelled picker selectedIndex is 0");
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
            waitPickerScrolling();
            tryCompare(spy, "count", 1);
            mouseClick(linearLong, units.gu(1), units.gu(18));
            waitPickerScrolling();
            tryCompare(spy, "count", 2);
        }

        function test_6_pickerCircularChange() {
            var expectedList = picker.circular ? "Picker_WrapAround" : "Picker_Linear";
            verify(findChild(picker, expectedList) !== undefined, "Picker must use " + expectedList);

            picker.circular = !picker.circular;
            expectedList = picker.circular ? "Picker_WrapAround" : "Picker_Linear";
            verify(findChild(picker, expectedList) !== undefined, "circular changed, Picker must use " + expectedList);
        }

        function test_7_modelCropping() {
            var selected = 50;
            linearDynPicker.selectedIndex = selected;
            circularDynPicker.selectedIndex = selected;
            waitPickerScrolling();
            var linearList = getPickerList(linearDynPicker, true);
            var circularList = getPickerList(circularDynPicker, false);

            selected = 40;
            dynamicModel.remove(selected, dynamicModel.count - selected);
            waitPickerScrolling();

            compare(linearList.count, selected, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, selected - 1, "bad index of linearList");

            compare(circularList.count, selected , "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, selected - 1, "bad index of circularList");

            // remove from the middle
            selected = 10;
            dynamicModel.remove(selected, 10);
            waitPickerScrolling();

            compare(linearList.count, 30, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, 29, "bad index of linearList");

            compare(circularList.count, 30, "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, 29, "bad index of circularList");

            // move selection in front and remove from after
            linearDynPicker.selectedIndex = 10;
            circularDynPicker.selectedIndex = 10;
            dynamicModel.remove(20, 10);
            waitPickerScrolling();

            compare(linearList.count, 20, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, 10, "bad index of linearList");

            compare(circularList.count, 20, "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, 10, "bad index of circularList");

            // remove 6 items around selectedIndex
            dynamicModel.remove(5, 6);
            waitPickerScrolling();

            compare(linearList.count, 14, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, 4, "bad index of linearList");

            compare(circularList.count, 14, "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, 4, "bad index of circularList");
        }

        function test_8_modelReset() {
            var linearList = getPickerList(linearDynPicker, true);
            var circularList = getPickerList(circularDynPicker, false);
            dynamicModel.reset();
            waitPickerScrolling();

            compare(linearList.currentIndex, 0, "linear picker's itemList selection not reset");
            compare(circularList.currentIndex, 0, "circular picker's itemList selection not reset");

            compare(linearDynPicker.selectedIndex, 0, "linear picker's selection not reset");
            compare(circularDynPicker.selectedIndex, 0, "circular picker's selection not reset");
        }

        function test_9_modelClear() {
            var linearList = getPickerList(linearDynPicker, true);
            var circularList = getPickerList(circularDynPicker, false);
            dynamicModel.clear();
            waitPickerScrolling();

            compare(linearList.currentIndex, -1, "linear picker's itemList selection not reset");
            expectFailContinue("", "PathView issue: https://bugreports.qt-project.org/browse/QTBUG-35400");
            compare(circularList.currentIndex, -1, "circular picker's itemList selection not reset");

            compare(linearDynPicker.selectedIndex, -1, "linear picker's selection not reset");
            expectFailContinue("", "PathView issue: https://bugreports.qt-project.org/browse/QTBUG-35400");
            compare(circularDynPicker.selectedIndex, -1, "circular picker's selection not reset");
        }
    }
}
