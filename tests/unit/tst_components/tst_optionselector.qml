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
import Ubuntu.Test 0.1

UbuntuTestCase {
     name: "OptionSelectorAPI"

     function test_expanded() {
         var listContainer = findChild(optionSelector, "listContainer");

         optionSelector.expanded = false;
         compare(listContainer.isExpanded, false, "isExpanded should be true if list is an expanded one");

         optionSelector.expanded = true;
         compare(listContainer.isExpanded, true, "isExpanded should be false if list is an expanded one");
     }

     function test_states_and_signal() {
         var listContainer = findChild(optionSelector, "listContainer");

         signalSpy.signalName = "scroll";
         compare(signalSpy.count, 0);

         optionSelector.expanded = false;
         compare(listContainer.state, "collapsed", "state should be collapsed");

         optionSelector.expanded = true;
         compare(listContainer.state, "expanded", "state should be expanded");

         //Check if scroll signal was fired after expansion.
         compare(signalSpy.count, 2);
     }

     function test_text() {
         compare(optionSelector.text,"","text is '' by default")
         var newText = "Hello World!";
         optionSelector.text = newText;
         compare(optionSelector.text, newText, "set/get");
     }

     function test_selectedIndex() {
        compare(optionSelector.selectedIndex, 0, "selectedIndex is 0 by default");
     }

     function test_values() {
         compare(optionSelector.values,undefined,"values is undefined by default")
         var newValues = ["value1","value2","value3"];
         optionSelector.model = newValues;
         compare(optionSelector.model, newValues, "set/get");
     }

     function test_custom_model_delegate() {
         compare(optionSelector.model, undefined, "model is undefined by default");
         optionSelector.model = customModel;
         optionSelector.delegate = selectorDelegate;
         compare(optionSelector.model, customModel, "Model wasn't set correctly.");
         compare(optionSelector.delegate, selectorDelegate, "Delegate hasn't been set correctly");
     }

     Rectangle {
         id: testItem
     }

     OptionSelector {
         id: optionSelector

         SignalSpy {
             id: signalSpy
             target: optionSelector
         }
     }

     OptionSelector {
         id: optionSelectorCustomModel
     }

     Component {
         id: selectorDelegate

         OptionSelectorDelegate { text: name }
     }

     ListModel {
         id: customModel
         ListElement { name: "Value 1" }
         ListElement { name: "Value 2" }
         ListElement { name: "Value 3" }
         ListElement { name: "Value 4" }
     }
}
