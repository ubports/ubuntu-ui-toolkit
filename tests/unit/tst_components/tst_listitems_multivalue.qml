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
     name: "ListItemsMultiValueAPI"

     function test_text() {
         compare(listItemMultiValue.text,"","text is '' by default")
         var newText = "Hello World!"
         listItemMultiValue.text = newText
         compare(listItemMultiValue.text,newText,"set/get")
     }

     function test_values() {
         compare(listItemMultiValue.values,undefined,"values is undefined by default")
         var newValues = ["value1","value2","value3"]
         listItemMultiValue.values = newValues
         compare(listItemMultiValue.values,newValues,"set/get")
     }

     ListItem.MultiValue {
         id: listItemMultiValue
     }
}
