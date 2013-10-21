# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import unittest

import mock
from autopilot import input, platform

from ubuntuuitoolkit import emulators, tests


class UbuntuUIToolkitEmulatorBaseTestCase(tests.QMLStringAppTestCase):

    def test_pointing_device(self):
        self.assertIsInstance(self.app.pointing_device, input.Pointer)

    @unittest.skipIf(platform.model() != 'Desktop', 'Desktop only')
    def test_pointing_device_in_desktop(self):
        self.assertIsInstance(self.app.pointing_device._device, input.Mouse)

    @unittest.skipIf(platform.model() == 'Desktop', 'Phablet only')
    def test_pointing_device_in_phablet(self):
        self.assertIsInstance(self.app.pointing_device._device, input.Touch)


class MainViewTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
}
""")

    def test_main_view_custom_emulator(self):
        self.assertIsInstance(self.main_view, emulators.MainView)

    def test_get_header_without_header(self):
        header = self.main_view.get_header()
        self.assertFalse(header.visible)

    def test_toolbar_custom_emulator(self):
        toolbar = self.main_view.get_toolbar()
        self.assertIsInstance(toolbar, emulators.Toolbar)
        self.assertFalse(toolbar.opened)

    def test_get_tabs_without_tabs(self):
        error = self.assertRaises(
            AssertionError, self.main_view.get_tabs)
        self.assertEqual(
            error.message, 'The MainView has no Tabs.')

    def test_switch_to_next_tab_without_tabs(self):
        header = self.main_view.get_header()
        error = self.assertRaises(
            AssertionError, header.switch_to_next_tab)
        self.assertEqual(
            error.message, 'The MainView has no Tabs.')


class PageTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Page {
        title: "Test title"
    }
}
""")

    def test_header_custom_emulator(self):
        header = self.main_view.get_header()
        self.assertIsInstance(header, emulators.Header)
        self.assertTrue(header.visible)
        self.assertEqual(header.title, "Test title")


class ToolbarTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(50)
    height: units.gu(50)

    Page {

        Label {
            id: "label"
            objectName: "clicked_label"
            anchors.centerIn: parent
            text: "Button not clicked."
        }

        tools: ToolbarItems {
            ToolbarButton {
                objectName: "buttonName"
                action: Action {
                    text: "buttonText"
                    onTriggered: label.text = "Button clicked."
                }
            }
            locked: false
            opened: false
        }
    }
}
""")

    def test_open_toolbar(self):
        toolbar = self.main_view.open_toolbar()
        self.assertTrue(toolbar.opened)
        self.assertFalse(toolbar.animating)

    def test_opened_toolbar_is_not_opened_again(self):
        toolbar = self.main_view.open_toolbar()
        with mock.patch.object(
                self.main_view.pointing_device, 'drag') as mock_drag:
            self.main_view.open_toolbar()

        self.assertFalse(mock_drag.called)
        self.assertTrue(toolbar.opened)

    def test_close_toolbar(self):
        toolbar = self.main_view.open_toolbar()
        self.main_view.close_toolbar()
        self.assertFalse(toolbar.opened)
        self.assertFalse(toolbar.animating)

    def test_closed_toolbar_is_not_closed_again(self):
        with mock.patch.object(
                self.main_view.pointing_device, 'drag') as mock_drag:
            self.main_view.close_toolbar()

        self.assertFalse(mock_drag.called)
        self.assertFalse(self.main_view.get_toolbar().opened)

    def test_click_toolbar_button(self):
        label = self.app.select_single('Label', objectName='clicked_label')
        self.assertNotEqual(label.text, 'Button clicked.')
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('buttonName')
        self.assertEqual(label.text, 'Button clicked.')

    def test_click_unexisting_button(self):
        toolbar = self.main_view.open_toolbar()
        error = self.assertRaises(
            ValueError, toolbar.click_button, 'unexisting')
        self.assertEqual(
            error.message, 'Button with objectName "unexisting" not found.')


class TabsTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Tabs {
        id: tabs
        Tab {
            objectName: "tab1"
            title: "Tab1"
            Page {
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "Test1"
                    }
                }
            }
        }
        Tab {
            objectName: "tab2"
            title: "Tab2"
            Page {
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "Test2"
                    }
                }
            }
        }
        Tab {
            objectName: "tab3"
            title: "Tab3"
            Page {
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "Test3"
                    }
                }
            }
        }
    }
}
""")

    def test_tabs_custom_emulator(self):
        self.assertIsInstance(self.main_view.get_tabs(), emulators.Tabs)

    def test_get_current_tab(self):
        tabs = self.main_view.get_tabs()
        self.assertEqual(tabs.get_current_tab().title, 'Tab1')

    def test_switch_to_next_tab_from_header(self):
        header = self.main_view.get_header()
        header.switch_to_next_tab()
        current_tab = self.main_view.get_tabs().get_current_tab()
        self.assertEqual(current_tab.title, 'Tab2')

    def test_switch_to_next_tab_from_main_view(self):
        current_tab = self.main_view.switch_to_next_tab()
        self.assertEqual(current_tab.title, 'Tab2')

    def test_switch_to_next_tab_from_last(self):
        last_tab_index = self.main_view.get_tabs().get_number_of_tabs() - 1
        self.main_view.switch_to_tab_by_index(last_tab_index)
        current_tab = self.main_view.switch_to_next_tab()
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_tab_by_index(self):
        current_tab = self.main_view.switch_to_tab_by_index(2)
        self.assertEqual(current_tab.title, 'Tab3')
        current_tab = self.main_view.switch_to_tab_by_index(1)
        self.assertEqual(current_tab.title, 'Tab2')
        current_tab = self.main_view.switch_to_tab_by_index(0)
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_opened_tab_is_not_opened_again(self):
        with mock.patch.object(
                emulators.Header, 'switch_to_next_tab') as mock_switch:
            current_tab = self.main_view.switch_to_tab_by_index(0)

        self.assertFalse(mock_switch.called)
        self.assertEqual(current_tab.title, 'Tab1')

    def test_get_number_of_tabs(self):
        tabs = self.main_view.get_tabs()
        self.assertEqual(tabs.get_number_of_tabs(), 3)

    def test_swith_to_tab_by_index_out_of_range(self):
        last_tab_index = self.main_view.get_tabs().get_number_of_tabs() - 1
        error = self.assertRaises(
            IndexError, self.main_view.switch_to_tab_by_index,
            last_tab_index + 1)
        self.assertEqual(error.message, 'Tab index out of range.')

    def test_switch_to_previous_tab_from_first(self):
        current_tab = self.main_view.switch_to_previous_tab()
        self.assertEqual(current_tab.title, 'Tab3')

    def test_switch_to_previous_tab_not_from_first(self):
        self.main_view.switch_to_tab_by_index(1)
        current_tab = self.main_view.switch_to_previous_tab()
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_tab_by_object_name(self):
        current_tab = self.main_view.switch_to_tab('tab3')
        self.assertEqual(current_tab.title, 'Tab3')
        current_tab = self.main_view.switch_to_tab('tab2')
        self.assertEqual(current_tab.title, 'Tab2')
        current_tab = self.main_view.switch_to_tab('tab1')
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_unexisting_tab(self):
        error = self.assertRaises(
            ValueError, self.main_view.switch_to_tab, 'unexisting')
        self.assertEqual(
            error.message, 'Tab with objectName "unexisting" not found.')


class ActionSelectionPopoverTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Button {
        objectName: "open_popover"
        text: "Open Popover"
        onClicked: testActionsPopover.show();
    }

    Label {
        id: "label"
        objectName: "clicked_label"
        anchors.centerIn: parent
        text: "Button not clicked."
    }

    ActionSelectionPopover {
        objectName: "test_actions_popover"
        id: testActionsPopover
        actions: ActionList {
            Action {
                text: "Action one"
                onTriggered: label.text = "Button clicked."
            }
        }
    }
}
""")

    def test_action_selection_popover_emulator(self):
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        self.assertIsInstance(popover, emulators.ActionSelectionPopover)

    def test_click_action_select_popover_button(self):
        label = self.app.select_single('Label', objectName='clicked_label')
        self.assertNotEqual(label.text, 'Button clicked.')
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        popover.click_button_by_text('Action one')
        self.assertEqual(label.text, 'Button clicked.')

    def _open_popover(self):
        open_button = self.main_view.select_single(
            'Button', objectName='open_popover')
        self.pointing_device.click_object(open_button)

    def test_click_unexisting_button(self):
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            ValueError, popover.click_button_by_text, 'unexisting')
        self.assertEqual(
            error.message, 'Button with text "unexisting" not found.')

    def test_click_button_with_closed_popover(self):
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            AssertionError, popover.click_button_by_text, 'Action one')
        self.assertEqual(
            error.message, 'The popover is not open.')


TEST_QML_WITH_CHECKBOX = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Item {
        CheckBox {
            checked: false
            objectName: "test_checkbox"
        }
    }
}
""")


TEST_QML_WITH_SWITCH = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Item {
        Switch {
            checked: false
            objectName: "test_switch"
        }
    }
}
""")


class ToggleTestCase(tests.QMLStringAppTestCase):

    scenarios = [
        ('checkbox', dict(
            test_qml=TEST_QML_WITH_CHECKBOX, objectName='test_checkbox')),
        ('switch', dict(
            test_qml=TEST_QML_WITH_SWITCH, objectName='test_switch'))
    ]

    def setUp(self):
        super(ToggleTestCase, self).setUp()
        self.toggle = self.main_view.select_single(
            emulators.CheckBox, objectName=self.objectName)

    def test_toggle_emulator(self):
        self.assertIsInstance(self.toggle, emulators.CheckBox)

    def test_check_toggle(self):
        self.assertFalse(self.toggle.checked)
        self.toggle.check()
        self.assertTrue(self.toggle.checked)

    def test_check_toggle_already_checked(self):
        self.toggle.check()
        with mock.patch.object(input.Pointer, 'click_object') as mock_click:
            self.toggle.check()
        self.assertFalse(mock_click.called)

    def test_uncheck_toggle(self):
        self.toggle.check()
        self.toggle.uncheck()
        self.assertFalse(self.toggle.checked)

    def test_uncheck_toggle_already_unchecked(self):
        with mock.patch.object(input.Pointer, 'click_object') as mock_click:
            self.toggle.uncheck()
        self.assertFalse(mock_click.called)


class PageStackTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    PageStack {
        id: pageStack
        Component.onCompleted: push(page0)

        Page {
            id: page0
            title: "Page 0"
            visible: false

            Button {
                objectName: "go_to_page1"
                text: "Go to page 1"
                onClicked: pageStack.push(page1)
            }
        }

        Page {
            id: page1
            title: "Page 1"
            visible: false
        }
    }
}
""")

    def setUp(self):
        super(PageStackTestCase, self).setUp()
        self.header = self.main_view.get_header()
        self.assertEqual(self.header.title, 'Page 0')

    def test_open_page(self):
        self._go_to_page1()
        self.assertEqual(self.header.title, 'Page 1')

    def _go_to_page1(self):
        button = self.main_view.select_single(
            'Button', objectName='go_to_page1')
        self.pointing_device.click_object(button)

    def test_go_back(self):
        self._go_to_page1()
        self.main_view.go_back()
        self.assertEqual(self.header.title, 'Page 0')
