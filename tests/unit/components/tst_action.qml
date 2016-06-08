/*
 * Copyright 2013 Canonical Ltd.
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
     name: "ActionAPI"

     TestUtil {
         id: util
     }

     function contains(list, entry) {
         for (var i = 0; i < list.length; i++) {
             if (list[i] == entry) {
                 return true;
             }
         }
         return false;
     }

     function cleanup() {
         triggeredSignalSpy.target = action;
         triggeredSignalSpy.clear();
         context1.active = false;
         context2.active = false;
     }

     function initTestCase() {
         compare(action.text, "", "text is empty string set by default")
         compare(action.iconSource, "", "iconSource is empty string by default")
         compare(action.iconName, "", "iconName is empty string by default")
     }

     function test_iconSource() {
         compare(action.iconSource, "", "iconSource is empty string by default")
         var newIconSource = Qt.resolvedUrl("../../../examples/ubuntu-ui-toolkit-gallery/small_avatar.png")
         action.iconSource = newIconSource
         compare(action.iconSource, newIconSource, "iconSource can be set")
         action.iconSource = ""
         compare(action.iconSource, "", "iconSource can be unset")
     }

     function test_iconName() {
         compare(action.iconName, "", "iconName is empty string by default")
         var newIconName = "compose"
         action.iconName = newIconName
         compare(action.iconName, newIconName, "iconName can be set")
         action.iconName = ""
         compare(action.iconName, "", "iconName can be unset")
     }

     function test_trigger() {
         compare(triggeredSignalSpy.count, 0)
         action.triggered(null);
         compare(triggeredSignalSpy.count, 1)
     }

     function test_signal_triggered_exists() {
         compare(triggeredSignalSpy.valid, true, "triggered signal exists")
     }

     function test_valid_value_type_data() {
         return [
            {tag: "None", type: Action.None, param: undefined},
            {tag: "String", type: Action.String, param: "test"},
            {tag: "Integer", type: Action.Integer, param: 100},
            {tag: "Bool", type: Action.Bool, param: true},
            {tag: "Real", type: Action.Real, param: 12.34},
            {tag: "Object - QtObject", type: Action.Object, param: object},
            {tag: "Object - Item", type: Action.Object, param: item},
         ];
     }
     function test_valid_value_type(data) {
         valueType.parameterType = data.type;
         valueType.trigger(data.param);
         valueTypeSpy.wait();
         compare(valueType.parameter, data.param, "Test " + data.tag + " result differs");
         valueTypeSpy.clear();
     }

     function test_invalid_value_type_data() {
         return [
            {tag: "None", type: Action.None, param: 120},
            {tag: "String", type: Action.String, param: object},
            {tag: "Integer", type: Action.Integer, param: "100"},
            {tag: "Bool", type: Action.Bool, param: item},
            {tag: "Real", type: Action.Real, param: undefined},
            {tag: "Object - QtObject", type: Action.Object, param: true},
            {tag: "Object - Item", type: Action.Object, param: "item"},
         ];
     }
     function test_invalid_value_type(data) {
         valueType.parameterType = data.type;
         valueType.trigger(data.param);
         valueTypeSpy.wait();
         compare(valueType.parameter, undefined, "Test " + data.tag + " did not fail");
         valueTypeSpy.clear();
     }

     function test_actionmanager() {
         verify(manager.globalContext, "Global context is not defined");
         compare(manager.localContexts.length, 2, "Invalid number of local contexts defined");
     }

     function test_globalcontext_actions() {
         compare(manager.globalContext.actions.length, 3, "Global context action count must be a sum of all manager's actions' counts");
     }

     function ignoreQMLWarning(message) {
         ignoreWarning(util.callerFile() + message);
     }

     function test_activate_contexts_data() {
         return [
             {tag: "Activate context1", active: [context1], inactive: [context2]},
             {tag: "Activate context2", active: [context2], inactive: [context1]},
             {tag: "Activate context1, context2", active: [context1, context2], inactive: []},
         ];
     }
     function test_activate_contexts(data) {
         for (var i = 0; i < data.active.length; i++) {
            data.active[i].active = true;
         }
         for (var i = 0; i < data.active.length; i++) {
            verify(data.active[i].active, "Context activation error");
         }
         for (var i = 0; i < data.inactive.length; i++) {
            verify(!data.inactive[i].active, "Context deactivation error");
         }
     }

     function test_overloaded_action_trigger_data() {
         return [
             {tag: "parametered override without parameter", action: suppressTrigger, invoked: true},
             {tag: "parametered override with parameter", action: suppressTrigger, value: 1, type: Action.Integer, invoked: true},
             {tag: "paremeterless override without parameter", action: override, invoked: true},
             {tag: "paremeterless override with parameter", action: override, value: 1, type: Action.Integer, invoked: true},
         ];
     }
     function test_overloaded_action_trigger(data) {
         data.action.invoked = false;
         data.action.parameterType = Action.None;
         testItem.action = data.action;
         if (data.value) {
             data.action.parameterType = data.type;
             testItem.trigger(data.value);
         } else {
             testItem.trigger(data.value);
         }
         compare(data.action.invoked, data.invoked);
     }

     Action {
         id: action
     }
     Action {
         id: valueType
         property var parameter
         onTriggered: parameter = value
     }

     QtObject {
         id: object
     }
     Item {
         id: item
     }

     SignalSpy {
         id: triggeredSignalSpy
         target: action
         signalName: "triggered"
     }
     SignalSpy {
         id: valueTypeSpy
         target: valueType
         signalName: "triggered"
     }
     SignalSpy {
         id: textSpy
         target: action
         signalName: "textChanged"
     }

     ActionManager {
         id: manager
     }

     ActionManager {
         id: manager2
         Action {
         }
         Action {
         }
         Action {
         }
     }

     ActionContext {
         id: context1
     }
     ActionContext {
         id: context2
     }

     Action {
         id: suppressTrigger
         property bool invoked: false
         // we must override the parametered version as Button connects to the parametered version
         function trigger(v) { invoked = true }
     }

     Action {
         id: override
         property bool invoked: false
         function trigger() { invoked = true }
     }

     ActionItem {
         id: testItem
     }

}
