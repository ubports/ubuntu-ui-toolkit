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
     name: "ListItemsBaseAPI"

     function test_fallbackIconSource() {
         expectFail("","https://bugs.launchpad.net/tavastia/+bug/1076762")
         compare(listItemBase.fallbackIconSource,undefined,"fallbackIconSource is not set by default")
     }

     function test_icon() {
         ignoreWarning('WARNING: ListItems.Base.icon is DEPRECATED. Use iconName and iconSource instead.')
         compare(listItemBase.icon,undefined,"icon is not set by default")

         // test with url
         var newIcon = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
         listItemBase.icon = newIcon
         compare(listItemBase.icon,newIcon,"set/get from url")
     }

     function test_iconFrame() {
         compare(listItemBase.iconFrame,true,"iconFrame is true by default")
     }

     function test_progression() {
         compare(listItemBase.progression,false,"progression is false by default")
     }

     Rectangle {
         id: testItem
     }

     ListItem.Base {
         id: listItemBase
     }
}
