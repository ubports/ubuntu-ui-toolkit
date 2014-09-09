/*
 * Copyright 2014 Canonical Ltd.
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

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Column {
        anchors.fill: parent
        UbuntuListView {
            id: listView
            clip: true
            width: parent.width
            height: parent.height / 3
            delegate: ListItem {
                objectName: "listItem" + index
            }
        }
        // override default leading
        UbuntuListView {
            id: customListView1
            clip: true
            width: parent.width
            height: parent.height / 3
            model: numberModel
            leadingOptions: ListItemOptions {
                Action {
                    iconName: "edit"
                }
            }
            delegate: ListItem {
                objectName: "listItem" + index
            }
        }
        // inline option list takes precedence (overrides UbuntuListView's default)
        UbuntuListView {
            id: customListView2
            clip: true
            width: parent.width
            height: parent.height / 3
            model: numberModel
            delegate: ListItem {
                objectName: "listItem" + index
                leadingOptions: ListItemOptions {
                    Action {
                        iconName: "edit"
                    }
                }
            }
        }
    }

    property int numberModel: 10
    property var arrayModel: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    property ListModel objectModel: ListModel {
        function fill() {
            clear();
            for (var i = 0; i < 10; i++) {
                append({data: "ListElement #" + i})
            }
        }
    }

    UbuntuTestCase {
        name: "ListItem with UbuntuListView"
        when: windowShown

        SignalSpy {
            id: actionSpy
            signalName: "triggered";
        }

        function init() {
            numberModel = 10;
            arrayModel = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
            objectModel.fill();
        }
        function cleanup() {
            listView.model = undefined;
            actionSpy.target = null;
            actionSpy.clear();
        }

        function test_leading_option_is_delete_alone() {
            // set a model to the listView
            listView.model = numberModel;
            waitForRendering(listView, 800);
            var listItem = findChild(listView, "listItem0");
            verify(listItem, "Cannot get list item at index 0");
            // tug the leadingOptions in
            flick(listItem.contentItem, centerOf(listItem.contentItem).x, centerOf(listItem.contentItem).y, listItem.contentItem.width / 2, 0);
            // leading options panel must be created
            verify(listItem.leadingOptions.panelItem, "Panel of LeadingOptions cannot be reached!");
            compare(listItem.leadingOptions.options.length, 1, "More options in the default leadingOptions than it should be!");
            compare(listItem.leadingOptions.options[0].iconName, "delete", "The default leading option is not 'delete'");
        }

        function test_delete_item_data() {
            return [
                {tag: "Number model", model: numberModel, index: 3, resultModelSize: 9 },
                {tag: "Array model", model: arrayModel, index: 4, resultModelSize: 9 },
                {tag: "Object model", model: objectModel, index: 2, resultModelSize: 9 },
            ];
        }
        function test_delete_item(data) {
            listView.model = data.model;
            waitForRendering(listView, 800);
            // navigate to teh list item
            listView.positionViewAtIndex(data.index, ListView.Contain);
            // perform delete by tugging the leading panel in
            var listItem = findChild(listView, "listItem" + data.index);
            verify(listItem, "Cannot get list item at index " + data.index);
            flick(listItem.contentItem, centerOf(listItem.contentItem).x, centerOf(listItem.contentItem).y, listItem.contentItem.width / 2, 0);

            // watch action triggers
            actionSpy.target = listItem.leadingOptions.options[0];
            // select the option on the leadingOptions
            var option = findChild(listItem.leadingOptions.panelItem, "list_option_0");
            verify(option, "Option cannot be reached");
            mouseClick(listItem, centerOf(option).x, centerOf(option).y);
            actionSpy.wait();

            // check model size by comparing the listview count
            compare(listView.count, data.resultModelSize, "Item had not been deleted!");
        }

        function test_default_optionlist_override_data() {
            return [
                {tag: "Default leadingOption overridden", view: customListView1},
                {tag: "Inline leadingOption has precedence over default", view: customListView2},
            ];
        }
        function test_default_optionlist_override(data) {
            var listItem = findChild(data.view, "listItem0");
            verify(listItem, "Cannot get list item at index 0");
            compare(listItem.leadingOptions.options.length, 1, "More than 1 option in the leadingOptions list");
            compare(listItem.leadingOptions.options[0].iconName, "edit", "Default UbuntuListItem option not overridden!");
        }
    }
}
