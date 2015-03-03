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
import Ubuntu.Components.ListItems 1.0 as ListItem

TestCase {
     name: "ListItemsValueSelectorAPI"

     function test_expanded() {
        compare(listItemValueSelector.expanded,false,"expanded is false by default")
     }

     function test_fallbackIconSource() {
         expectFail("","https://bugs.launchpad.net/tavastia/+bug/1076762")
         compare(listItemValueSelector.fallbackIconSource,undefined,"fallbackIconSource is not set by default")
     }

     function test_icon() {
         ignoreWarning('WARNING: ListItems.Base.icon is DEPRECATED. Use iconName and iconSource instead.')
         compare(listItemValueSelector.icon,undefined,"icon is not set by default")

         // test with url
         var newIcon = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
         listItemValueSelector.icon = newIcon
         compare(listItemValueSelector.icon,newIcon,"set/get from url")
     }

     function test_iconFrame() {
         compare(listItemValueSelector.iconFrame,true,"iconFrame is true by default")
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

     ListItem.ValueSelector {
         id: listItemValueSelector
     }
}
