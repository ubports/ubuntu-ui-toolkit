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
                Label {text: modelData}
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
        function test_0_itemWidth() {
            compare(picker.itemWidth, picker.width, "itemWidth is same as picker's width by default");
        }
        function test_0_itemHeight() {
            compare(picker.itemHeight, units.gu(4.5), "itemHeight is 4.5 GU by default");
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
            mouseClick(linearLong, units.gu(1), units.gu(15));
            wait(500);
            tryCompare(spy, "count", 1);
            mouseClick(linearLong, units.gu(1), units.gu(15));
            wait(500);
            tryCompare(spy, "count", 2);
        }

        function test_6_pickerCircularChange() {
            var expectedList = picker.circular ? "Picker_WrapAround" : "Picker_Linear";
            verify(findChild(picker, expectedList) !== undefined, "Picker must use " + expectedList);

            picker.circular = !picker.circular;
            expectedList = picker.circular ? "Picker_WrapAround" : "Picker_Linear";
            verify(findChild(picker, expectedList) !== undefined, "circular changed, Picker must use " + expectedList);
        }

        function test_7_itemHeight_itemWidth() {
            linearShort.itemHeight = units.gu(6);
            compare(linearShort.itemList.currentItem.height, units.gu(6), "linear delegate height same as itemHeight");
            linearShort.itemWidth = units.gu(6);
            compare(linearShort.itemList.currentItem.width, units.gu(6), "linear delegate width same as itemWidth");

            // itemWidth is omitted for larger width than the Picker's width
            linearShort.itemWidth = linearShort.width + units.gu(1);
            compare(linearShort.itemList.currentItem.width, linearShort.width, "larger itemWidth than Pickewr width is omitted");
        }

        function test_8_modelCropping() {
            var selected = 50;
            linearDynPicker.selectedIndex = selected;
            circularDynPicker.selectedIndex = selected;
            // wait few milisecods till views are scrolling
            wait(500);
            compare(linearDynPicker.selectedIndex, linearDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for linearList");
            compare(circularDynPicker.selectedIndex, circularDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for circularList");

            selected = 40;
            dynamicModel.remove(selected, dynamicModel.count - selected);
            wait(500);

            compare(linearDynPicker.itemList.count, selected, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, linearDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for linearList");
            compare(linearDynPicker.selectedIndex, selected - 1, "bad index of linearList");

            compare(circularDynPicker.itemList.count, selected , "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, circularDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for circularList");
            compare(circularDynPicker.selectedIndex, selected - 1, "bad index of circularList");

            // remove from the middle
            selected = 10;
            dynamicModel.remove(selected, 10);
            wait(500);

            compare(linearDynPicker.itemList.count, 30, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, linearDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for linearList");
            compare(linearDynPicker.selectedIndex, 29, "bad index of linearList");

            compare(circularDynPicker.itemList.count, 30, "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, circularDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for circularList");
            compare(circularDynPicker.selectedIndex, 29, "bad index of circularList");

            // move selection in front and remove from after
            linearDynPicker.selectedIndex = 10;
            circularDynPicker.selectedIndex = 10;
            dynamicModel.remove(20, 10);
            wait(500);

            compare(linearDynPicker.itemList.count, 20, "bad removal from linearList")
            compare(linearDynPicker.selectedIndex, linearDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for linearList");
            compare(linearDynPicker.selectedIndex, 10, "bad index of linearList");

            compare(circularDynPicker.itemList.count, 20, "bad removal from circularList")
            compare(circularDynPicker.selectedIndex, circularDynPicker.itemList.currentIndex, "selectedIndex and itemList.currentIndex differ for circularList");
            compare(circularDynPicker.selectedIndex, 10, "bad index of circularList");
        }

        function test_9_modelReset() {
            dynamicModel.reset();
            wait(500);

            compare(linearDynPicker.itemList.currentIndex, 0, "linear picker's itemList selection not reset");
            compare(circularDynPicker.itemList.currentIndex, 0, "circular picker's itemList selection not reset");

            compare(linearDynPicker.selectedIndex, 0, "linear picker's selection not reset");
            compare(circularDynPicker.selectedIndex, 0, "circular picker's selection not reset");
        }

        function test_9_modelClear() {
            dynamicModel.clear();
            wait(500);

            compare(linearDynPicker.itemList.currentIndex, -1, "linear picker's itemList selection not reset");
            expectFailContinue("", "PathView issue: https://bugreports.qt-project.org/browse/QTBUG-35400");
            compare(circularDynPicker.itemList.currentIndex, -1, "circular picker's itemList selection not reset");

            compare(linearDynPicker.selectedIndex, -1, "linear picker's selection not reset");
            expectFailContinue("", "PathView issue: https://bugreports.qt-project.org/browse/QTBUG-35400");
            compare(circularDynPicker.selectedIndex, -1, "circular picker's selection not reset");

            // restore model and selections
            dynamicModel.reset();
            wait(500);
//            linearDynPicker.selectedIndex = 0;
//            circularDynPicker.selectedIndex = 0;
        }
    }
}
