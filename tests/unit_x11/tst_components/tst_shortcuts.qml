/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: root
    width: 400
    height: 600

    Action {
        id: action
    }
    Action {
        id: other
        shortcut: 'Ctrl+G'
    }

    TestUtil {
        id: util
    }

    UbuntuTestCase {
        id: testCase
        name: "Shortcuts"
        when: windowShown

        function initTestCase() {
        }

        function init() {
        }
        function cleanup() {
            spy.clear();
            shortcutSpy.clear();
        }

        SignalSpy {
            id: spy
            signalName: 'triggered'
            target: action
        }

        function ignoreQMLWarning(message) {
            ignoreWarning(util.callerFile() + message);
        }

        function test_shortcut_triggered_data() {
            return [
                { tag: 'Multiple modifiers and letter', shortcut: 'Ctrl+Shift+Alt+A', key: Qt.Key_A, mod: Qt.ControlModifier + Qt.ShiftModifier + Qt.AltModifier },
                { tag: 'Modifier and letter', shortcut: 'Ctrl+A', key: Qt.Key_A, mod: Qt.ControlModifier },
                { tag: 'Single letter', shortcut: 'E', key: Qt.Key_E, mod: Qt.NoModifier },
                { tag: 'StandardKey', shortcut: StandardKey.Copy, key: Qt.Key_C, mod: Qt.ControlModifier }
            ];
        }
        function test_shortcut_triggered(data) {
            action.shortcut = data.shortcut;
            spy.clear();
            keyClick(data.key, data.mod);
            spy.wait();
        }

        function test_shortcut_invalid_data() {
            return [
                { tag: 'Typo', shortcut: 'Ctr+F' },
                { tag: 'Number', shortcut: 1234567890 },
                { tag: "Empty string", shortcut: "" }
            ];
        }
        function test_shortcut_invalid(data) {
            ignoreQMLWarning(':27:5: QML Action: Invalid shortcut: ');
            action.shortcut = data;
        }

        function test_shortcut_duplicate() {
            ignoreQMLWarning(':30:5: QML Action: Ambiguous shortcut: Ctrl+G');
            action.shortcut = other.shortcut;
            keyClick(Qt.Key_G, Qt.ControlModifier);
        }

        SignalSpy {
            id: shortcutSpy
            signalName: "shortcutChanged"
        }

        function test_shortcut_reset_bug1518420() {
            shortcutSpy.target = action;
            action.shortcut = "Ctrl+A";
            shortcutSpy.wait(200);
            shortcutSpy.clear();
            action.shortcut = undefined;
            shortcutSpy.wait(200);
            shortcutSpy.target = null;
        }

        function test_mnemonic_data() {
            return [
                {tag: "HW keyboard, valid A", kbd: true, text: "C&all", displayText: "C<u>a</u>ll", key: Qt.Key_A, xfail: false},
                {tag: "HW keyboard, valid S", kbd: true, text: "&Save & Exit", displayText: "<u>S</u>ave & Exit", key: Qt.Key_S, xfail: false},
                {tag: "HW keyboard, valid space", kbd: true, text: "Save & Exit", displayText: "Save <u> </u>Exit", key: Qt.Key_Space, xfail: false},

                {tag: "HW keyboard, invalid", kbd: true, text: "&&Call", displayText: "&&Call", key: Qt.Key_Asterisk, xfail: true},

                {tag: "no HW keyboard", kbd: false, text: "&Call", displayText: "Call", key: Qt.Key_C, xfail: false},
            ];
        }
        function test_mnemonic(data) {
            QuickUtils.keyboardAttached = data.kbd;
            action.text = data.text;
            if (!data.kbd && QuickUtils.keyboardAttached) {
                skip("Cannot test this case: " + data.tag);
            }
            compare(action.text, data.displayText);
            // shortcut
            keyClick(data.key, Qt.AltModifier);
            if (data.xfail) {
                expectFail(data.tag, "invalid mnemonic");
            }
            spy.wait(200);
        }

        SignalSpy {
            id: textSpy
            target: action
            signalName: "textChanged"
        }

        function test_mnemonic_displaytext() {
            QuickUtils.keyboardAttached = false;
            if (QuickUtils.keyboardAttached) {
                skip("the test needs to be able to detach the keyboard");
            }
            action.text = "&Call";
            textSpy.clear();
            QuickUtils.keyboardAttached = true;
            if (!QuickUtils.keyboardAttached) {
                skip("the test needs to be able to attach the keyboard");
            }
            textSpy.wait(200);
        }
    }
}
