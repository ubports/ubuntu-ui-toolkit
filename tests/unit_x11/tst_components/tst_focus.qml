/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.Pickers 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 1.3

Item {
    id: main
    width: units.gu(50); height: units.gu(100)

    property bool hasOSK: false

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
        Button {
            id: dummy
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
        StyledItem {
            id: disabledButton
            enabled: false
            width: units.gu(20)
            height: units.gu(6)
        }
        ComboButton {
            id: comboButton
            Rectangle {
                height: comboButton.comboListHeight
                color: "blue"
            }
        }
        Button {
            id: popoverTest
            text: "Popovers"
            property Item popover
            property Component popoverComponent
            onClicked: {
                popover = PopupUtils.open(popoverComponent)
            }
        }
    }

    Component {
        id: dialogComponent
        Dialog {
            id: dialog
            title: "TestDialog"
            Button {
                text: "close"
                onClicked: PopupUtils.close(dialog)
            }
        }
    }

    Component {
        id: popoverComponent
        Popover {
            id: popover
            contentWidth: units.gu(20)
            contentHeight: item.height
            ListItem.Standard {
                id: item
                text: "close"
                onClicked: PopupUtils.close(popover)
            }
        }
    }

    UbuntuTestCase {
        name: "FocusingTests"
        when: windowShown

        SignalSpy {
            id: popupCloseSpy
            signalName: "onDestruction"
        }

        function initTestCase() {
            main.hasOSK = QuickUtils.inputMethodProvider !== ""
            textField.forceActiveFocus();
        }
        function cleanup() {
            // empty event buffer
            wait(200);
            popupCloseSpy.clear();
            popupCloseSpy.target = null;
        }

        // make this as the very first test executed
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

        function test_tab_focus_data() {
            return [
                {tag: "TextField", from: dummy, to: textField, key: Qt.Key_Tab},
                {tag: "TextField(back)", from: textField, to: dummy, key: Qt.Key_Backtab},
                {tag: "TextArea", from: textField, to: textArea, key: Qt.Key_Tab},
                {tag: "TextArea(back)", from: textArea, to: textField, key: Qt.Key_Backtab},
                {tag: "Button(back)", from: button, to: textArea, key: Qt.Key_Backtab},
            ];
        }
        function test_tab_focus(data) {
            data.from.forceActiveFocus();
            verify(data.from.focus, "Source component is not focused");
            keyClick(data.key);
            waitForRendering(data.to, 200);
            verify(data.to.focus, "Target component is not focused");
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
            compare(dropdownButton.focus, true, "Dropdown button hasn't got focused!");
            compare(comboButton.focus, true, "ComboButton hasn't been focused!");
            comboButton.expanded = false;
            waitForRendering(comboButton);
        }

        function test_popover_refocus_data() {
            return [
                {tag: "Dialog", component: dialogComponent},
                {tag: "Popover", component: popoverComponent},
            ];
        }

        function test_popover_refocus(data) {
            popoverTest.popoverComponent = data.component;
            var center = centerOf(popoverTest);
            mouseClick(popoverTest, center.x, center.y);
            verify(popoverTest.focus, "Button focus not gained.");
            waitForRendering(popoverTest.popover);
            popupCloseSpy.target = popoverTest.popover.Component;

            var closeButton = findChildWithProperty(popoverTest.popover, "text", "close");
            verify(closeButton, "Close button not accessible");
            center = centerOf(closeButton);
            mouseClick(closeButton, center.x, center.y);
            verify(!popoverTest.focus, "Button focus not lost.");
            popupCloseSpy.wait();
            verify(popoverTest.focus, "Button focus not restored.");
        }

        function test_disabled_component_does_not_focus() {
            mousePress(disabledButton, centerOf(disabledButton).x, centerOf(disabledButton).y);
            compare(disabledButton.focus, false, "Disabled component shoudl not focus");
        }
    }
}
