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
     name: "ListItemsEmptyAPI"

     function test_highlightWhenPressed() {
         compare(listItemEmpty.highlightWhenPressed,true,"highlightWhenPressed is true by default")
         listItemEmpty.highlightWhenPressed = false
         compare(listItemEmpty.highlightWhenPressed,false,"set/get")
     }

     function test_hovered() {
        compare(listItemEmpty.hovered,false,"hovered is false by default")
     }

     function test_pressed() {
        compare(listItemEmpty.pressed,false,"pressed is false by default")
     }

     function test_selected() {
        compare(listItemEmpty.selected,false,"selected is false by default")
     }

     function test_showDivider() {
        compare(listItemEmpty.showDivider,true,"showDivider is true by default")
        listItemEmpty.showDivider = false
        compare(listItemEmpty.showDivider,false,"set/get")
     }

     ListItem.Empty {
         id: listItemEmpty
     }
}
