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
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Test 0.1

Item {
    width: 400
    height: 400

    Column {
        anchors {
            left: parent.left
            right: parent.right
        }
        ListItem.ItemSelector {
            id: selector

            text: "TEST"
            delegate: selectorDelegate
            model: customModel
            expanded: true
        }

        OptionSelectorDelegate {
            id: testDelegate

            text: "TEST"
            subText: "test"
            icon: "../../resources/optionselector/test.png"
            constrainImage: true
        }
    }

    Component {
        id: selectorDelegate

        OptionSelectorDelegate {
            text: name
            subText: description
            icon: image
            constrainImage: true
        }
    }

    ListModel {
        id: customModel
        ListElement { name: "Name 1"; description: "Description 1"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Name 2"; description: "Description 2"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Name 3"; description: "Description 3"; image: "../../resources/optionselector/test.png" }
        ListElement { name: "Name 4"; description: "Description 4"; image: "../../resources/optionselector/test.png" }
    }

    SignalSpy {
        id: clickedSignal
        target: selector
        signalName: "delegateClicked"
    }

    SignalSpy {
        id: expansionSignal
        target: selector
        signalName: "expansionCompleted"
    }

    UbuntuTestCase {
         name: "ItemSelectorAPI"
         when: windowShown

         function test_expanded() {
             var listContainer = findChild(selector, "listContainer");

             selector.expanded = false;
             compare(listContainer.isExpanded, false, "isExpanded should be true if list is an expanded one");
             compare(listContainer.state, "collapsed", "state should be collapsed");

             selector.expanded = true;
             compare(listContainer.isExpanded, true, "isExpanded should be false if list is an expanded one");
             compare(listContainer.state, "expanded", "state should be expanded");
         }

         function test_text() {
             var newText = "Hello World!";
             selector.text = newText;
             compare(selector.text, newText, "set/get");
         }

         function test_selectedIndex() {
            compare(selector.selectedIndex, 0, "selectedIndex is 0 by default");
         }

         function test_model() {
             selector.model = undefined;
             var newValues = ["value0","value1","value2","value3"];
             selector.model = newValues;
             compare(selector.model, newValues, "set/get");
         }

         function test_custom_model_delegate() {
             compare(selector.model, customModel, "Model wasn't set correctly.");
             compare(selector.delegate, selectorDelegate, "Delegate hasn't been set correctly");
         }

         function test_image_constraint() {
            var image = findChild(testDelegate, "icon");
            compare(image.height, testDelegate.height);
         }

         function test_signal() {
             var listContainer = findChild(selector, "listContainer");
             mouseMove(selector, 100, 100);
             mouseClick(selector, 100, 100, Qt.LeftButton);
             tryCompare(expansionSignal, "count", 1);
             compare(clickedSignal.count, 1, "Clicked not emitted.");
         }
    }
}
