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
        }

        ListItem.ItemSelector {
            id: baseSelector
        }
    }

    Component {
        id: selectorDelegate

        OptionSelectorDelegate { text: name; subText: description }
    }

    ListModel {
        id: customModel
        ListElement { name: "Name 1"; description: "Description 1" }
        ListElement { name: "Name 2"; description: "Description 2" }
        ListElement { name: "Name 3"; description: "Description 3" }
        ListElement { name: "Name 4"; description: "Description 4" }
    }

    SignalSpy {
        id: clickedSignal
        target: selector
        signalName: "delegateClicked"
    }

    SignalSpy {
        id: scrollSignal
        target: selector
        signalName: "scroll"
    }

    UbuntuTestCase {
         name: "ItemSelectorAPI"
         when: windowShown

         function test_signals() {
             mouseMove(selector, 0, 0);
             mouseClick(selector, 0, 0, Qt.LeftButton);
             wait(2000)
             compare(clickedSignal.count, 1, "Clicked not emitted.");
             compare(scrollSignal.count, 1, "Scroll not emitted.");
         }

         function test_expanded() {
             var listContainer = findChild(baseSelector, "listContainer");

             baseSelector.expanded = false;
             compare(listContainer.isExpanded, false, "isExpanded should be true if list is an expanded one");
             compare(listContainer.state, "collapsed", "state should be collapsed");

             baseSelector.expanded = true;
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
             baseSelector.model = undefined;
             compare(baseSelector.model,undefined,"values is undefined by default")
             var newValues = ["value0","value1","value2","value3"];
             baseSelector.model = newValues;
             compare(baseSelector.model, newValues, "set/get");
         }

         function test_custom_model_delegate() {
             compare(baseSelector.model, undefined, "model is undefined by default");
             baseSelector.model = customModel;
             baseSelector.delegate = selectorDelegate;
             compare(baseSelector.model, customModel, "Model wasn't set correctly.");
             compare(baseSelector.delegate, selectorDelegate, "Delegate hasn't been set correctly");
         }
    }
}
