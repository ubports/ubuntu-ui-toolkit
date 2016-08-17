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
     name: "AbstractButtonAPI"

     function test_action() {
         compare(absButton.action, null,"Action is null by default")
         absButton.action = action1
         compare(absButton.action, action1, "Action can be set")
         var numTriggers = action1.triggerCount
         absButton.clicked()
         compare(action1.triggerCount, numTriggers+1, "Button clicked triggers action")
         absButton.action = null
     }

     function test_hovered() {
        compare(absButton.hovered,false,"Hovered is boolean and false by default")
     }

     function test_pressed() {
        compare(absButton.pressed,false,"Pressed is boolean and false by default")
     }

     function test_signal_clicked() {
         signalSpy.signalName = "clicked";
         compare(signalSpy.valid,true,"clicked signal exists")
     }

     function test_signal_pressAndHold() {
         signalSpy.signalName = "pressAndHold";
         compare(signalSpy.valid,true,"pressAndHold signal exists")
     }

     AbstractButton {
         id: absButton
         SignalSpy {
             id: signalSpy
             target: parent
         }
     }

     Action {
         id: action1
         property int triggerCount: 0
         onTriggered: triggerCount++
     }
}
