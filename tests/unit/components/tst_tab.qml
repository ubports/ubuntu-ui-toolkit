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
import Ubuntu.Components 1.1

TestCase {
     name: "TabAPI"

     function test_iconSource() {
        compare(tab.iconSource,"","iconSource is not set by default")
        var newIcon = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
        tab.iconSource = newIcon
        compare(tab.iconSource,newIcon,"Can set/get iconSource")
     }

     function test_page() {
        compare(tab.page,null,"page is null by default")
     }

     function test_title() {
         compare(tab.title,"","is not set by default")
         var newTitle = "Hello World!"
         tab.title = newTitle
         compare(tab.title,newTitle,"can set/get")
     }

     function test_index() {
         compare(tab.index, -1, "is -1 by default")
     }

     Tab {
         id: tab
     }
}
