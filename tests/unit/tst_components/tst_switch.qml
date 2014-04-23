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
     name: "SwitchAPI"

     function test_checked() {
         compare(switchComponent.checked,false,"checked is false by default")

         switchComponent.checked = true
         compare(switchComponent.checked,true,"Can set/get checked")
     }

     function test_hovered() {
        compare(switchComponent.hovered,false,"Hovered is boolean and false by default")
     }

     function test_pressed() {
        compare(switchComponent.pressed,false,"Pressed is boolean and false by default")
     }

     Switch {
         id: switchComponent
     }
}
