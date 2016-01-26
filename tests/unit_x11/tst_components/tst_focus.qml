/*
 * Copyright 2014-2016 Canonical Ltd.
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
            primaryItem: MouseArea {
                id: textFieldPrimaryItem
                height: parent.height
                width: height
            }
            secondaryItem: TextField {
                id: textFieldSecondaryItem
                height: parent.height
                width: height
            }
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
        }
        Switch {
            id: switchbox
        }
        Button {
            id: dummy2
        }
        ActionBar {
            id: actions
            actions: [
                Action {
                    iconName: 'share'
                    objectName: 'actionBarShare'
                },
                Action {
                    iconName: 'starred'
                }
            ]
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
        Button {
            id: disabledButton
            text: 'Disabled'
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
            property var popover
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
            id: buttonTriggerSpy
            signalName: "onTriggered"
        }

        SignalSpy {
            id: popupCloseSpy
            signalName: "onDestruction"
        }

        SignalSpy {
            id: textFieldPrimaryItemClickedSpy
            target: textFieldPrimaryItem
            signalName: "onClicked"
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

        function test_tab_focus_data() {
            return [
                // (Shift)Tab via keyboard
                {tag: "TextField", from: dummy, to: textField, key: Qt.Key_Tab},
                {tag: "TextField(back)", from: textField, to: dummy, key: Qt.Key_Backtab},
                {tag: "TextArea", from: textField, to: textArea, key: Qt.Key_Tab},
                {tag: "TextArea(back)", from: textArea, to: textField, key: Qt.Key_Backtab},
                {tag: "Button(back)", from: button, to: textArea, key: Qt.Key_Backtab},
                {tag: "CheckBox", from: checkbox, to: switchbox, key: Qt.Key_Tab},
                {tag: "CheckBox", from: switchbox, to: checkbox, key: Qt.Key_Backtab},
                {tag: "Switch", from: switchbox, to: dummy2, key: Qt.Key_Tab},
                {tag: "Switch(back)", from: dummy2, to: switchbox, key: Qt.Key_Backtab},
                /* FIXME: Figure out how to test ActionBar delegate focus
                {tag: "ActionBar", from: 'actionBarShare_button', to: picker, key: Qt.Key_Tab},
                {tag: "ActionBar(back)", from: picker, to: 'actionBarShare_button', key: Qt.Key_Backtab},
                */
                // Left click/ tap
                {tag: "TextField(click)", from: dummy, to: textField, key: Qt.LeftButton},
                {tag: "TextArea(click)", from: dummy, to: textArea, key: Qt.LeftButton},
                // FIXME: lp#1368390: Buttons shouldn't grab input focus on click
                {tag: "Button(click)", from: dummy, to: button, key: Qt.LeftButton},
                {tag: "CheckBox(click)", from: dummy, to: checkbox, key: Qt.LeftButton},
                {tag: "Switch(click)", from: dummy, to: switchbox, key: Qt.LeftButton}
            ];
        }
        function test_tab_focus(data) {
            data.from.forceActiveFocus();
            verify(data.from.activeFocus, "Source component is not focused");
            if (data.key == Qt.LeftButton) {
                verify(data.to.activeFocusOnPress, "Target doesn't take focus on click");
                mouseClick(data.to, centerOf(data.to).x, centerOf(data.to).y);
            } else {
                verify(data.to.activeFocusOnTab, "Target doesn't take keyboard focus");
                keyClick(data.key);
            }
            waitForRendering(data.to, 500);
            verify(!data.from.activeFocus, "Source component still keeps focus");
            verify(data.to.activeFocus, "Target component is not focused");
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

        function test_textfield_primary_and_secondary_items_keeps_focus() {
            textFieldPrimaryItemClickedSpy.clear();
            textField.forceActiveFocus();
            verify(textField.activeFocus);
            verify(!textFieldSecondaryItem.activeFocusOnPress);

            var center = centerOf(textFieldPrimaryItem);
            mouseClick(textFieldPrimaryItem, center.x, center.y);
            compare(textFieldPrimaryItemClickedSpy.count, 1);
            verify(textField.activeFocus);
            verify(!textFieldPrimaryItem.activeFocus);

            center = centerOf(textFieldSecondaryItem);
            mouseClick(textFieldSecondaryItem, center.x, center.y);
            verify(textField.activeFocus);
            verify(!textFieldSecondaryItem.activeFocus);
        }

        function test_combo_button_dropdown_focuses_component() {
            textField.forceActiveFocus();
            var dropdownButton = findChild(comboButton, "combobutton_dropdown");
            verify(dropdownButton, "dropdown button is not accessible?");

            var center = centerOf(dropdownButton);
            mouseClick(dropdownButton, center.x, center.y);
            waitForRendering(comboButton);
            // FIXME: lp#1368390: Buttons shouldn't grab input focus on click
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
            verify(popoverTest.popover !== undefined, "No popover created");
            waitForRendering(popoverTest.popover);
            verify(!popoverTest.focus, "Button focus not lost.");
            var foreground = findChild(popoverTest.popover, "popover_foreground");
            verify(foreground, "Popover foreground not ready");
            verify(foreground.focus, "Popover focus not gained. Focus went to %1".arg(window.activeFocusItem));
            popupCloseSpy.target = popoverTest.popover.Component;

            var closeButton = findChildWithProperty(popoverTest.popover, "text", "close");
            verify(closeButton, "Close button not accessible");
            center = centerOf(closeButton);
            mouseClick(closeButton, center.x, center.y);
            verify(!popoverTest.focus, "Button focus not lost.");
            popupCloseSpy.wait();
            verify(popoverTest.focus, "Button focus not restored.");
        }

        function test_button_trigger_via_keyboard_data() {
            return [
                {tag: "Enter", key: Qt.Key_Enter},
                {tag: "Return", key: Qt.Key_Return},
                {tag: "Space", key: Qt.Key_Space},
            ];
        }
        function test_button_trigger_via_keyboard(data) {
            buttonTriggerSpy.target = button;
            button.forceActiveFocus();
            keyClick(data.key);
            waitForRendering(button);
            buttonTriggerSpy.wait();
        }

        function test_disabled_component_does_not_focus() {
            verify(disabledButton.activeFocusOnPress, "Disabled component doesn't take focus on click");
            mousePress(disabledButton, centerOf(disabledButton).x, centerOf(disabledButton).y);
            compare(disabledButton.focus, false, "Disabled component shoudl not focus");
        }
    }
}
