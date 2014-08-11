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

import QtQuick 2.2
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem

Item {
    id: main
    width: units.gu(50); height: units.gu(100)

    property bool hasOSK: QuickUtils.inputMethodProvider !== ""

    Flow {
        anchors {
            fill: parent
            margins: units.gu(0.5)
        }
        spacing: units.gu(1)

        UbuntuListView {
            id: listView
            width: units.gu(50)
            height: units.gu(20)
            clip: true
            model: 10
            delegate: ListItem.Standard {
                text: "Whatever"
            }
        }
        TextField {
            id: textField
            text: "This is a text field with some text handling focus"
        }
        TextArea {
            id: textArea
            text: "This is a text area with some text handling focus"
        }
        Button {
            id: button
            text: "Press me"
        }
        CheckBox {
            id: checkbox
            text: "Check me"
        }
        Switch {
            id: switchBox
        }
        Picker {
            id: picker
            model: 10
        }
        Picker {
            id: roundPicker
            model: 10
            circular: true
        }
        Button {
            id: pickerPanel
            property date date: new Date()
            text: Qt.formatDateTime(date, "yyyy/MMMM")
            onClicked: PickerPanel.openDatePicker(pickerPanel, "date", "Years|Months")
        }
        Slider {
            id: slider
        }
        ComboButton {
            id: comboButton
            Rectangle {
                height: comboButton.comboListHeight
                color: "blue"
            }
        }
    }

    UbuntuTestCase {
        name: "FocusingTests"
        when: windowShown

        function initTestCase() {
            textField.forceActiveFocus();
        }
        function cleanup() {
            // empty event buffer
            wait(200);
        }

        // make this as teh very first test executed
        function test_0_transfer_focus_data() {
            return [
                {tag: "TextArea", previousFocused: textField, focusOn: textArea, clickToDismiss: false},
                {tag: "Button", previousFocused: textArea, focusOn: button, clickToDismiss: false},
                {tag: "Checkbox", previousFocused: button, focusOn: checkbox, clickToDismiss: false},
                {tag: "Switch", previousFocused: checkbox, focusOn: switchBox, clickToDismiss: false},
                {tag: "Picker - linear", previousFocused: switchBox, focusOn: picker, clickToDismiss: false},
                {tag: "Picker - circular", previousFocused: picker, focusOn: roundPicker, clickToDismiss: false},
                {tag: "PickerPanel", previousFocused: roundPicker, focusOn: pickerPanel, clickToDismiss: true},
                {tag: "UbuntuListView", previousFocused: pickerPanel, focusOn: listView, clickToDismiss: false},
                {tag: "Slider", previousFocused: listView, focusOn: slider, clickToDismiss: false},
                {tag: "ComboButton", previousFocused: slider, focusOn: comboButton, clickToDismiss: false},
            ];
        }
        function test_0_transfer_focus(data) {
            // perform mouse press on
            mouseClick(data.focusOn, centerOf(data.focusOn).x, centerOf(data.focusOn).y);
            compare(data.previousFocused.focus, false, "Previous focus is still set!");
            compare(data.focusOn.focus, true, data.tag + " is not focused!");
            if (data.clickToDismiss) {
                mouseClick(main, 0, 0);
            }
            waitForRendering(data.focusOn, 200);
        }

        function test_hide_osk_when_pickerpanel_opens() {
            if (!main.hasOSK) {
                skip("This functionality can be tested with OSK only.");
            }

            textField.forceActiveFocus();
            verify(textField.focus, "TextField is not focused");
            verify(Qt.inputMethod.visible, "OSK is hidden");
            mouseClick(pickerPanel, centerOf(pickerPanel).x, centerOf(pickerPanel).y);
            verify(!Qt.inputMethod.visible, "OSK is visible still!");
            // remove panel/popover
            mouseClick(main, 0, 0);
        }

        function test_textfield_clear_button_keeps_focus() {
            textField.forceActiveFocus();
            var text = textField.text;

            var clearButton = findChild(textField, "clear_button");
            verify(clearButton, "clearButton is not accessible!");
            var center = centerOf(clearButton);
            mouseClick(clearButton, center.x, center.y);
            compare(textField.text, "", "Text had not been cleared?");
            compare(textField.focus, true, "Focus had been stolen from text input");

            //restore
            textField.text = text;
        }

        function test_combo_button_dropdown_focuses_component() {
            textField.forceActiveFocus();
            var dropdownButton = findChild(comboButton, "combobutton_dropdown");
            verify(dropdownButton, "dropdown button is not accessible?");

            var center = centerOf(dropdownButton);
            mouseClick(dropdownButton, center.x, center.y);
            waitForRendering(comboButton);
            compare(dropdownButton.focus, false, "dropdown button got focused!");
            compare(comboButton.focus, true, "ComboButton had not been focused");
            comboButton.expanded = false;
        }
    }
}
