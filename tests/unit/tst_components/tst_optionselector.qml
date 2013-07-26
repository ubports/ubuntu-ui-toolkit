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
         var listContainer = findChild(listItemValueSelector, "listContainer")

         listItemValueSelector.expanded = false
         compare(listContainer.isExpanded, false, "isExpanded should be true if list is an expanded one")

         listItemValueSelector.expanded = true
         compare(listContainer.isExpanded, true, "isExpanded should be false if list is an expanded one")
     }

     function test_states() {
         var listContainer = findChild(listItemValueSelector, "listContainer")

         listItemValueSelector.expanded = false
         compare(listContainer.state, "collapsed", "state should be collapsed")

         listItemValueSelector.expanded = true
         compare(listContainer.state, "expanded", "state should be expanded")
     }

     function test_text() {
         compare(listItemValueSelector.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemValueSelector.text = newText
         compare(listItemValueSelector.text,newText,"set/get")
     }

     function test_selectedIndex() {
        compare(listItemValueSelector.selectedIndex,0,"selectedIndex is 0 by default")
     }

     function test_values() {
         compare(listItemValueSelector.values,undefined,"values is undefined by default")
         var newValues = ["value1","value2","value3"]
         listItemValueSelector.values = newValues
         compare(listItemValueSelector.values,newValues,"set/get")
     }

     Rectangle {
         id: testItem
     }

     OptionSelector {
         id: listItemValueSelector
     }
}
