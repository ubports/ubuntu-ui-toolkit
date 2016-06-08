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
import Ubuntu.Components.Popups 1.3

Item {
    id: main
    width: units.gu(50); height: units.gu(100)

    property bool hasOSK: false

    property bool keysReleased
    Keys.onReleased: keysReleased = true

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
            delegate: ListItem {
                Label {
                    text: "Whatever"
                }
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
                Component.onCompleted: activeFocusOnTab = false
            }
        }
        TextArea {
            id: textArea
            text: "This is a text area with some text handling focus"
        }
        Item {
            BottomEdgeHint {
                id: bottomEdgeHint
            }
        }
        Button {
            id: button
            text: "Press me"
            property bool override: false
            Keys.onReleased: event.accepted = override
        }
        ListItem {
            id: listItem
            property bool override: false
            Keys.onReleased: event.accepted = override
            Label {
                text: "Cuddle me"
            }
        }
        CheckBox {
            id: checkbox
        }
        Switch {
            id: switchbox
        }
        Slider {
            id: slider
        }
        Sections {
            id: sections
            actions: [
                Action { text: "Section 1" },
                Action { text: "Section 2" },
                Action { text: "Section 3" }
            ]
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
            property var popover: null
            property Component popoverComponent: popoverComponent
            onClicked: {
                if (popoverTest.popoverComponent) {
                    popoverTest.popover = PopupUtils.open(popoverTest.popoverComponent);
                } else {
                    popoverTest.popover.show();
                }
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
            ListItem {
                Label {
                    id: item
                    text: "close"
                }
                onClicked: PopupUtils.close(popover)
            }
        }
    }

    Dialog {
        id: dialogItem
        title: "TestDialog"
        Button {
            text: "close"
            onClicked: dialogItem.hide()
        }
    }

    Popover {
        id: popoverItem
        contentWidth: units.gu(20)
        contentHeight: item.height
        ListItem {
            Label {
                id: item
                text: "close"
            }
            onClicked: popoverItem.hide();
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
            popoverTest.popover = null;
            popoverTest.popoverComponent = popoverComponent;
            popupCloseSpy.clear();
            popupCloseSpy.target = null;
            popupCloseSpy.signalName = "onDestruction";
            buttonTriggerSpy.target = null;
            buttonTriggerSpy.clear();
        }

        function test_tab_focus_data() {
            return [
                // (Shift)Tab via keyboard
                {tag: "TextField", from: dummy, to: textField, key: Qt.Key_Tab},
                {tag: "TextField(back)", from: textField, to: dummy, key: Qt.Key_Backtab},
                {tag: "TextArea", from: textField, to: textArea, key: Qt.Key_Tab},
                {tag: "TextArea(back)", from: textArea, to: textField, key: Qt.Key_Backtab},
                {tag: "BottomEdgeHint", from: textArea, to: bottomEdgeHint, key: Qt.Key_Tab},
                {tag: "BottomEdgeHint(back)", from: bottomEdgeHint, to: textArea, key: Qt.Key_Backtab},
                {tag: "Button", from: bottomEdgeHint, to: button, key: Qt.Key_Tab},
                {tag: "Button(back)", from: button, to: bottomEdgeHint, key: Qt.Key_Backtab},
                {tag: "ListItem", from: button, to: listItem, key: Qt.Key_Tab},
                {tag: "ListItem(back)", from: listItem, to: button, key: Qt.Key_Backtab},
                {tag: "CheckBox", from: checkbox, to: switchbox, key: Qt.Key_Tab},
                {tag: "CheckBox", from: switchbox, to: checkbox, key: Qt.Key_Backtab},
                {tag: "Switch", from: switchbox, to: slider, key: Qt.Key_Tab},
                {tag: "Switch(back)", from: slider, to: switchbox, key: Qt.Key_Backtab},
                {tag: "Slider", from: slider, to: sections, key: Qt.Key_Tab, keyFocusItem: "section_button_0"},
                {tag: "Slider(back)", from: sections, to: slider, key: Qt.Key_Backtab},
                // NOTE: Navigation INSIDE the sections using the arrow keys is tested
                //  in tst_sections.qml.
                {tag: "Sections", from: sections, to: dummy2, key: Qt.Key_Tab},
                {tag: "Sections(back)", from:dummy2, to: sections, key: Qt.Key_Backtab, keyFocusItem: "section_button_0"},
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
                {tag: "Switch(click)", from: dummy, to: switchbox, key: Qt.LeftButton},
                {tag: "Slider(click)", from: dummy, to: slider, key: Qt.LeftButton},
            ];
        }
        function test_tab_focus(data) {
            data.from.focus = true;
            verify(data.from.enabled, "Source component is invalid");
            verify(data.to.enabled, "Target component is invalid");
            verify(data.from.activeFocus, "Source component is not focused - focus is on %1"
                .arg(String(window.activeFocusItem).split("(")[0]));
            if (data.key == Qt.LeftButton) {
                verify(data.to.activeFocusOnPress, "Target doesn't take focus on click");
                mouseClick(data.to, centerOf(data.to).x, centerOf(data.to).y);
            } else {
                verify(data.to.activeFocusOnTab, "Target doesn't take keyboard focus");
                keyClick(data.key);

                if (data.keyFocusItem) {
                    var child = findChild(data.to, data.keyFocusItem);
                    verify(child, "Target does not have child with objectName "+data.keyFocusItem);
                    verify(child.keyNavigationFocus, "Target child does not have keyNavigationFocus");
                } else {
                    verify(data.to.keyNavigationFocus, "Target does not have keyNavigationFocus");
                }
            }
            waitForRendering(data.to, 500);
            verify(!data.from.activeFocus, "Source component still keeps focus");
            verify(data.to.activeFocus, "Target component is not focused - focus is on %1"
                .arg(String(window.activeFocusItem).split("(")[0]));
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
                        {   tag: "Dialog component",
                            component: dialogComponent,
                            item: null,
                            foreground_name: "dialogForeground",
                            bug: "1569979"
                        },
                        {   tag: "Popover component",
                            component: popoverComponent,
                            item: null,
                            foreground_name: "popover_foreground",
                            bug: "" // this is the only case without a bug
                        },
                        {   tag: "Dialog item",
                            component: null,
                            item: dialogItem,
                            foreground_name: "dialogForeground",
                            bug: "1569979 and 1569981"
                        },
                        {   tag: "Popover item",
                            component: null,
                            item: popoverItem,
                            foreground_name: "popover_foreground",
                            bug: "1569981"
                        },
            ];
        }

        function test_popover_refocus(data) {
            popoverTest.popoverComponent = data.component;
            popoverTest.popover = data.item;
            var center = centerOf(popoverTest);
            mouseClick(popoverTest, center.x, center.y);
            verify(popoverTest.popover !== undefined, "No popover created");
            waitForRendering(popoverTest.popover);
            verify(!popoverTest.focus, "Button focus not lost.");
            var foreground = findChild(popoverTest.popover, data.foreground_name);
            verify(foreground, "Popover foreground not ready");
            verify(foreground.focus, "Popover focus not gained. Focus went to %1".arg(window.activeFocusItem));

            if (data.item) {
                // popup will only be hidden, not destroyed.
                popupCloseSpy.signalName = "onVisibleChanged";
                popupCloseSpy.target = popoverTest.popover;
            } else {
                popupCloseSpy.signalName = "onDestruction";
                popupCloseSpy.target = popoverTest.popover.Component;
            }

            var closeButton = findChildWithProperty(popoverTest.popover, "text", "close");
            verify(closeButton, "Close button not accessible");
            center = centerOf(closeButton);
            mouseClick(closeButton, center.x, center.y);
            popupCloseSpy.wait();
            if (data.bug) {
                expectFailContinue(data.tag,
                                   "Incorrect focus restoration. See bug " + data.bug + ".");
            }
            verify(popoverTest.focus, "Button focus not restored.");
        }

        function test_trigger_via_keyboard_data() {
            return [
                {tag: "Button/Enter", key: Qt.Key_Enter, item: button, signalName: 'onTriggered'},
                {tag: "Button/Return", key: Qt.Key_Return, item: button, signalName: 'onTriggered'},
                {tag: "Button/Space", key: Qt.Key_Space, item: button, signalName: 'onTriggered'},
                {tag: "ListItem/Enter", key: Qt.Key_Enter, item: listItem, signalName: 'onClicked'},
                {tag: "ListItem/Return", key: Qt.Key_Return, item: listItem, signalName: 'onClicked'},
                {tag: "ListItem/Space", key: Qt.Key_Space, item: listItem, signalName: 'onClicked'},
            ];
        }
        function test_trigger_via_keyboard(data) {
            data.item.override = false;
            main.keysReleased = false;
            buttonTriggerSpy.signalName = data.signalName;
            buttonTriggerSpy.target = data.item;
            data.item.forceActiveFocus();
            keyClick(data.key);
            buttonTriggerSpy.wait();
            compare(main.keysReleased, false, "Parent didn't get Keys.onReleased");

            // Second attempt but the button will consume Keys.onReleased
            data.item.override = true;
            main.keysReleased = false;
            buttonTriggerSpy.clear();
            keyClick(data.key);
            expectFailContinue(data.tag, "Trigger should've been overridden");
            buttonTriggerSpy.wait();
            compare(main.keysReleased, false, "Parent didn't get Keys.onReleased");
        }

        function test_disabled_component_does_not_focus() {
            verify(disabledButton.activeFocusOnPress, "Disabled component doesn't take focus on click");
            mousePress(disabledButton, centerOf(disabledButton).x, centerOf(disabledButton).y);
            compare(disabledButton.focus, false, "Disabled component shoudl not focus");
        }
    }
}
