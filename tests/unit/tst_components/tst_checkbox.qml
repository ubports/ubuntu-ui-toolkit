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
     name: "CheckBoxAPI"

     function test_checked() {
         compare(checkbox.checked,false,"CheckBox.checked is false by default")

         checkbox.checked = true
         compare(checkbox.checked,true,"Can set/get checked")
     }

     function test_hovered() {
        compare(checkbox.hovered,false,"Hovered is boolean and false by default")
     }

     function test_pressed() {
        compare(checkbox.pressed,false,"Pressed is boolean and false by default")
     }

     function test_triggered() {
        checkbox.checked = false
        checkbox.clicked()
        signalSpy.wait()
        compare(checkbox.checked, true, "click flipped from unchecked to checked")
        checkbox.clicked()
        signalSpy.wait()
        compare(checkbox.checked, false, "click flipped from checked back to unchecked")

        // clicked and trigger both flip the checked state
        checkbox.trigger()
        signalSpy.wait()
        compare(checkbox.checked, true, "trigger flipped from unchecked to checked")
        checkbox.trigger()
        signalSpy.wait()
        compare(checkbox.checked, false, "trigger flipped from checked back to unchecked")
     }

     CheckBox {
         id: checkbox
         action: Action {
             enabled: true
             name: "check"
             text: "Check"
             parameterType: Action.Bool
         }
     }

     SignalSpy {
        id: signalSpy
        signalName: "triggered"
        target: checkbox
     }
}
