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
     name: "ActionItemAPI"

     SignalSpy {
         id: triggerSpy
         target: action1
         signalName: "triggered"
     }

     function initTestCase() {
         compare(item1.action, null, "action is null by default")
         compare(item1.text, "", "text is empty string set by default")
         compare(item1.iconSource, "", "iconSource is empty string by default")
         compare(item1.iconName, "", "iconSource is empty string by default")
     }

     function cleanup() {
         item1.action = null;
         triggerSpy.clear();
     }

     function test_action() {
         compare(item1.action, null,"Action is null by default")
         item1.action = action1
         compare(item1.action, action1, "Action can be set")
         compare(item1.text, action1.text, "text is automatically set to action text")
         compare(item1.iconSource, action1.iconSource, "iconSource is automatically set to action iconSource")
         item1.triggered(null)
         triggerSpy.wait(400);
     }

     // NOTE: This test must be run AFTER test_action(), otherwise setting the action will
     // not update the text
     function test_text() {
         compare(item1.text, "", "text is empty string by default")
         var newText = "new text"
         item1.text = newText
         compare(item1.text, newText, "text can be set")
         item1.text = ""
         compare(item1.text, "", "text can be unset")
     }

     // NOTE: This test must be run AFTER test_action(), otherwise setting the action will
     // will not update the iconSource
     function test_iconSource() {
         compare(item1.iconSource, "", "iconSource is empty string by default")
         var newIconSource = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
         item1.iconSource = newIconSource
         compare(item1.iconSource, newIconSource, "iconSource can be set")
         item1.iconSource = ""
         compare(item1.iconSource, "", "iconSource can be unset")
     }

     // NOTE: This test must be run AFTER test_action(), otherwise setting the action will
     // will not update the iconName
     function test_iconName() {
         compare(item1.iconName, "", "iconName is empty string by default")
         var newIconName = "compose"
         item1.iconName = newIconName
         compare(item1.iconName, newIconName, "iconName can be set")
         item1.iconName = ""
         compare(item1.iconName, "", "iconName can be unset")
     }

     function test_signal_triggered() {
         signalSpy.signalName = "triggered";
         compare(signalSpy.valid,true,"triggered signal exists")
     }

     ActionItem {
         id: item1
         SignalSpy {
             id: signalSpy
             target: parent
         }
     }

     Action {
         id: action1
         text: "actionText"
         iconSource: "imageURL"
     }
}
