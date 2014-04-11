# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014 Canonical Ltd.
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

import time
import unittest

import autopilot
from autopilot import input, platform
from autopilot.introspection import dbus
from testtools.matchers import GreaterThan, LessThan
try:
    from unittest import mock
except ImportError:
    import mock

from ubuntuuitoolkit import emulators, tests


class CheckAutopilotVersionTestCase(unittest.TestCase):

    def test_lower_version_should_raise_exception(self):
        with mock.patch.object(autopilot, 'version', '1.3'):
            self.assertRaises(
                emulators.ToolkitEmulatorException,
                emulators.check_autopilot_version)

    def test_required_version_should_succeed(self):
        with mock.patch.object(autopilot, 'version', '1.4'):
            emulators.check_autopilot_version()

    def test_higher_version_should_succeed(self):
        with mock.patch.object(autopilot, 'version', '1.5'):
            emulators.check_autopilot_version()


class UbuntuUIToolkitEmulatorBaseTestCase(tests.QMLStringAppTestCase):

    def test_pointing_device(self):
        self.assertIsInstance(self.app.pointing_device, input.Pointer)

    @unittest.skipIf(platform.model() != 'Desktop', 'Desktop only')
    def test_pointing_device_in_desktop(self):
        self.assertIsInstance(self.app.pointing_device._device, input.Mouse)

    @unittest.skipIf(platform.model() == 'Desktop', 'Phablet only')
    def test_pointing_device_in_phablet(self):
        self.assertIsInstance(self.app.pointing_device._device, input.Touch)

    def test_emulators_should_check_version_on_init(self):
        check_name = 'ubuntuuitoolkit.emulators.check_autopilot_version'
        with mock.patch(check_name, autospec=True) as mock_check:
            # Instantiate any emulator.
            self.main_view

        mock_check.assert_called_once_with()


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

    def test_open_toolbar(self):
        with mock.patch.object(emulators.Toolbar, 'open') as mock_open:
            self.main_view.open_toolbar()

        mock_open.assert_called_once_with()

    def test_close_toolbar(self):
        with mock.patch.object(emulators.Toolbar, 'close') as mock_close:
            self.main_view.close_toolbar()

        mock_close.assert_called_once_with()

    def test_open_toolbar_returns_the_toolbar(self):
        toolbar = self.main_view.open_toolbar()
        self.assertIsInstance(toolbar, emulators.Toolbar)

    def test_get_tabs_without_tabs(self):
        error = self.assertRaises(
            emulators.ToolkitEmulatorException, self.main_view.get_tabs)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_switch_to_next_tab_without_tabs(self):
        header = self.main_view.get_header()
        error = self.assertRaises(
            emulators.ToolkitEmulatorException, header.switch_to_next_tab)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')


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

    // Make sure that for these tests the toolbar starts closed.
    Component.onCompleted: {
        __propagated.toolbar.close();
    }

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
        }
    }
}
""")

    def setUp(self):
        super(ToolbarTestCase, self).setUp()
        self.toolbar = self.main_view.get_toolbar()
        # toolbar may be opened or closed now, depending on whether
        # the application has been deactivated and resumed already

    def test_open_toolbar(self):
        self.toolbar.open()
        self.assertTrue(self.toolbar.opened)
        self.assertFalse(self.toolbar.animating)

    def test_opened_toolbar_is_not_opened_again(self):
        self.toolbar.open()
        with mock.patch.object(
                self.main_view.pointing_device, 'drag') as mock_drag:
            self.toolbar.open()

        self.assertFalse(mock_drag.called)
        self.assertTrue(self.toolbar.opened)

    def test_close_toolbar(self):
        self.toolbar.open()
        self.toolbar.close()
        self.assertFalse(self.toolbar.opened)
        self.assertFalse(self.toolbar.animating)

    def test_closed_toolbar_is_not_closed_again(self):
        self.toolbar.close()
        with mock.patch.object(
                self.main_view.pointing_device, 'drag') as mock_drag:
            self.toolbar.close()

        self.assertFalse(mock_drag.called)
        self.assertFalse(self.toolbar.opened)

    def test_click_toolbar_button(self):
        self.toolbar.close()
        label = self.app.select_single('Label', objectName='clicked_label')
        self.assertNotEqual(label.text, 'Button clicked.')
        self.toolbar.open()
        self.toolbar.click_button('buttonName')
        self.assertEqual(label.text, 'Button clicked.')

    def test_click_unexisting_button(self):
        self.main_view.open_toolbar()
        error = self.assertRaises(
            emulators.ToolkitEmulatorException, self.toolbar.click_button,
            'unexisting')
        self.assertEqual(
            str(error), 'Button with objectName "unexisting" not found.')

    def test_click_button_on_closed_toolbar(self):
        self.toolbar.close()
        error = self.assertRaises(
            emulators.ToolkitEmulatorException, self.toolbar.click_button,
            'buttonName')
        self.assertEqual(
            str(error),
            'Toolbar must be opened before calling click_button().')


class TabsTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(70)
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
            emulators.ToolkitEmulatorException,
            self.main_view.switch_to_tab_by_index,
            last_tab_index + 1)
        self.assertEqual(str(error), 'Tab index out of range.')

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
            emulators.ToolkitEmulatorException, self.main_view.switch_to_tab,
            'unexisting')
        self.assertEqual(
            str(error), 'Tab with objectName "unexisting" not found.')


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
            emulators.ToolkitEmulatorException, popover.click_button_by_text,
            'unexisting')
        self.assertEqual(
            str(error), 'Button with text "unexisting" not found.')

    def test_click_button_with_closed_popover(self):
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            emulators.ToolkitEmulatorException, popover.click_button_by_text,
            'Action one')
        self.assertEqual(
            str(error), 'The popover is not open.')


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
        self.assertFalse(self.toggle.checked)

    def test_toggle_emulator(self):
        self.assertIsInstance(self.toggle, emulators.CheckBox)

    def test_check_toggle(self):
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

    def test_change_state_from_checked(self):
        self.toggle.check()
        self.toggle.change_state()
        self.assertFalse(self.toggle.checked)

    def test_change_state_from_unchecked(self):
        self.toggle.change_state()
        self.assertTrue(self.toggle.checked)

    def test_check_with_timeout(self):
        with mock.patch.object(
                emulators.CheckBox, 'change_state') as mock_change:
            self.toggle.check(timeout=1)

        mock_change.assert_called_once_with(1)

    def test_uncheck_with_timeout(self):
        self.toggle.check()
        with mock.patch.object(
                emulators.CheckBox, 'change_state') as mock_change:
            self.toggle.uncheck(timeout=1)

        mock_change.assert_called_once_with(1)

    def test_change_state_with_timeout(self):
        with mock.patch.object(self.toggle, 'pointing_device'):
            # mock the pointing device so the checkbox is not clicked.
            timestamp_before_call = time.time()
            self.assertRaises(AssertionError, self.toggle.change_state, 1)

        waiting_time = time.time() - timestamp_before_call
        self.assertThat(waiting_time, GreaterThan(1))
        self.assertThat(waiting_time, LessThan(2))


class QQuickListViewTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(48)
    height: units.gu(20)

    Page {

        Column {
            id: column
            width: units.gu(48)
            height: units.gu(20)

            Label {
                id: clickedLabel
                objectName: "clickedLabel"
                text: "No element clicked."
            }

            ListView {
                id: testListView
                objectName: "testListView"
                anchors.left: parent.left
                anchors.right: parent.right
                height: column.height - clickedLabel.paintedHeight
                clip: true
                model: 20

                delegate: ListItem.Standard {
                    objectName: "testListElement%1".arg(index)
                    text: "test list element %1".arg(index)
                    onClicked: clickedLabel.text = objectName
                    height: units.gu(5)
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(QQuickListViewTestCase, self).setUp()
        self.list_view = self.main_view.select_single(
            emulators.QQuickListView, objectName='testListView')
        self.label = self.main_view.select_single(
            'Label', objectName='clickedLabel')
        self.assertEqual(self.label.text, 'No element clicked.')

    def test_qquicklistview_emulator(self):
        self.assertIsInstance(self.list_view, emulators.QQuickListView)

    def test_click_element(self):
        self.list_view.click_element('testListElement0')
        self.assertEqual(self.label.text, 'testListElement0')

    def test_click_element_outside_view_below(self):
        # Click the first element out of view to make sure we are not scrolling
        # to the bottom at once.
        self.assertFalse(
            self.list_view._is_element_clickable('testListElement5'))

        self.list_view.click_element('testListElement5')
        self.assertEqual(self.label.text, 'testListElement5')

    def test_click_element_outside_view_above(self):
        self.list_view.click_element('testListElement9')

        # Click the first element out of view to make sure we are not scrolling
        # to the top at once.
        self.assertFalse(
            self.list_view._is_element_clickable('testListElement4'))

        self.list_view.click_element('testListElement4')
        self.assertEqual(self.label.text, 'testListElement4')

    def test_click_element_not_created_at_start(self):
        objectName = 'testListElement19'
        self.assertRaises(
            dbus.StateNotFoundError,
            self.list_view.select_single,
            objectName=objectName)
        self.list_view.click_element(objectName)

    def test_click_unexisting_element(self):
        error = self.assertRaises(
            emulators.ToolkitEmulatorException,
            self.list_view.click_element,
            'unexisting')
        self.assertEqual(
            str(error), 'List element with objectName "unexisting" not found.')


class QQuickListViewOutOfViewTestCase(tests.QMLStringAppTestCase):
    """Test that we can make elements visible when the list is out of view."""

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    width: units.gu(48)
    height: units.gu(20)

    Page {

        Flickable {

            Column {
                id: column
                width: units.gu(48)
                // The column height is greater than the main view height, so
                // the bottom of the list is out of view.
                height: units.gu(40)

                Label {
                    id: clickedLabel
                    objectName: "clickedLabel"
                    text: "No element clicked."
                }

                ListView {
                    id: testListView
                    objectName: "testListView"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: column.height - clickedLabel.paintedHeight
                    clip: true
                    model: 20

                    delegate: ListItem.Standard {
                        objectName: "testListElement%1".arg(index)
                        text: "test list element %1".arg(index)
                        onClicked: clickedLabel.text = objectName
                        height: units.gu(5)
                    }
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(QQuickListViewOutOfViewTestCase, self).setUp()
        self.list_view = self.main_view.select_single(
            emulators.QQuickListView, objectName='testListView')
        self.label = self.main_view.select_single(
            'Label', objectName='clickedLabel')
        self.assertEqual(self.label.text, 'No element clicked.')

    def test_click_element_outside_view_below(self):
        """Test that we can click an element that's out of view below.

        The list is also out of view, so we must scroll from the bottom of the
        main view.

        """
        # Test for http://pad.lv/1275060.
        # Click the first element out of view to make sure we are not scrolling
        # to the bottom at once.
        self.assertFalse(
            self.list_view._is_element_clickable('testListElement9'))

        self.list_view.click_element('testListElement9')
        self.assertEqual(self.label.text, 'testListElement9')


class SwipeToDeleteTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1


MainView {
    width: units.gu(48)
    height: units.gu(60)

    Page {

        ListModel {
            id: testModel

            ListElement {
                name: "listitem_destroyed_on_remove_with_confirm"
                label: "Item destroyed on remove with confirmation"
                confirm: true
            }
            ListElement {
                name: "listitem_destroyed_on_remove_without_confirm"
                label: "Item destroyed on remove without confirmation"
                confirm: false
            }
        }

        Column {
            anchors { fill: parent }

            Standard {
                objectName: "listitem_standard"
                confirmRemoval: true
                removable: true
                text: 'Slide to remove'
            }

            Empty {
                objectName: "listitem_empty"
            }

            Standard {
                objectName: "listitem_without_confirm"
                confirmRemoval: false
                removable: true
                text: "Item without delete confirmation"
            }

            ListView {
                anchors { left: parent.left; right: parent.right }
                height: childrenRect.height
                model: testModel

                delegate: Standard {
                    removable: true
                    confirmRemoval: confirm
                    onItemRemoved: testModel.remove(index)
                    text: label
                    objectName: name
                }
            }
        }
    }
}
""")

    def setUp(self):
        super(SwipeToDeleteTestCase, self).setUp()
        self._item = self.main_view.select_single(
            emulators.Standard, objectName='listitem_standard')
        self.assertTrue(self._item.exists())

    def test_supported_class(self):
        self.assertTrue(issubclass(
            emulators.Base, emulators.Empty))
        self.assertTrue(issubclass(
            emulators.ItemSelector, emulators.Empty))
        self.assertTrue(issubclass(
            emulators.Standard, emulators.Empty))
        self.assertTrue(issubclass(
            emulators.SingleControl, emulators.Empty))
        self.assertTrue(issubclass(
            emulators.MultiValue, emulators.Base))
        self.assertTrue(issubclass(
            emulators.SingleValue, emulators.Base))
        self.assertTrue(issubclass(
            emulators.Subtitled, emulators.Base))

    def test_standard_emulator(self):
        self.assertIsInstance(self._item, emulators.Standard)

    def test_swipe_item(self):
        self._item.swipe_to_delete()
        self.assertTrue(self._item.waitingConfirmationForRemoval)

    def test_swipe_item_to_right(self):
        self._item.swipe_to_delete('right')
        self.assertTrue(self._item.waitingConfirmationForRemoval)

    def test_swipe_item_to_left(self):
        self._item.swipe_to_delete('left')
        self.assertTrue(self._item.waitingConfirmationForRemoval)

    def test_swipe_item_to_wrong_direction(self):
        self.assertRaises(
            emulators.ToolkitEmulatorException,
            self._item.swipe_to_delete, 'up')

    def test_delete_item_moving_right(self):
        self._item.swipe_to_delete('right')
        self._item.confirm_removal()
        self.assertFalse(self._item.exists())

    def test_delete_item_moving_left(self):
        self._item.swipe_to_delete('left')
        self._item.confirm_removal()
        self.assertFalse(self._item.exists())

    def test_delete_non_removable_item(self):
        self._item = self.main_view.select_single(
            emulators.Empty, objectName='listitem_empty')
        self.assertRaises(
            emulators.ToolkitEmulatorException, self._item.swipe_to_delete)

    def test_confirm_removal_when_item_was_not_swiped(self):
        self.assertRaises(
            emulators.ToolkitEmulatorException, self._item.confirm_removal)

    def test_delete_item_without_confirm(self):
        item = self.main_view.select_single(
            emulators.Standard, objectName='listitem_without_confirm')
        item.swipe_to_delete()
        self.assertFalse(item.exists())

    def test_delete_item_with_confirmation_that_will_be_destroyed(self):
        item = self.main_view.select_single(
            emulators.Standard,
            objectName='listitem_destroyed_on_remove_with_confirm')
        item.swipe_to_delete()
        item.confirm_removal()
        self.assertFalse(item.exists())

    def test_delete_item_without_confirmation_that_will_be_destroyed(self):
        item = self.main_view.select_single(
            emulators.Standard,
            objectName='listitem_destroyed_on_remove_without_confirm')
        item.swipe_to_delete()
        self.assertFalse(item.exists())


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


class TextFieldTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Item {
        TextField {
            id: simpleTextField
            objectName: "simple_text_field"
        }
        TextField {
            id: textFieldWithoutClearButton
            objectName: "text_field_without_clear_button"
            hasClearButton: false
            anchors.top: simpleTextField.bottom
        }
    }
}
""")

    def setUp(self):
        super(TextFieldTestCase, self).setUp()
        self.simple_text_field = self.main_view.select_single(
            emulators.TextField, objectName='simple_text_field')

    def test_text_field_emulator(self):
        self.assertIsInstance(self.simple_text_field, emulators.TextField)

    def test_write(self):
        self.simple_text_field.write('test')
        self.assertEqual(self.simple_text_field.text, 'test')

    def test_clear_with_clear_button(self):
        self.simple_text_field.write('test')
        self.simple_text_field.clear()
        self.assertEqual(self.simple_text_field.text, '')

    def test_clear_without_clear_button(self):
        text_field = self.main_view.select_single(
            emulators.TextField, objectName='text_field_without_clear_button')
        text_field.write('test')
        text_field.clear()
        self.assertEqual(text_field.text, '')

    def test_clear_and_write(self):
        self.simple_text_field.write('test1')
        self.simple_text_field.write('test2')
        self.assertEqual(self.simple_text_field.text, 'test2')

    def test_write_without_clear(self):
        self.simple_text_field.write('test1')
        self.simple_text_field.write('test2', clear=False)
        self.assertEqual(self.simple_text_field.text, 'test1test2')

    def test_write_without_clear_writes_at_the_end(self):
        self.simple_text_field.write(
            'long text that will fill more than half of the text field.')
        self.simple_text_field.write('append', clear=False)
        self.assertEqual(
            self.simple_text_field.text,
            'long text that will fill more than half of the text field.append')

    def test_is_empty(self):
        self.assertTrue(self.simple_text_field.is_empty())
        self.simple_text_field.write('test')
        self.assertFalse(self.simple_text_field.is_empty())


class ComposerSheetTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Button {
        objectName: "openComposerSheetButton"
        text: "Open Composer Sheet"
        onClicked: PopupUtils.open(testComposerSheet);
    }

    Label {
        id: "label"
        objectName: "actionLabel"
        anchors.centerIn: parent
        text: "No action taken."
    }

    Component {
        id: testComposerSheet
        ComposerSheet {
            id: sheet
            objectName: "testComposerSheet"
            onCancelClicked: {
                label.text = "Cancel selected."
            }
            onConfirmClicked: {
                label.text = "Confirm selected."
            }
        }
    }
}
""")

    def setUp(self):
        super(ComposerSheetTestCase, self).setUp()
        self.label = self.main_view.select_single(
            'Label', objectName='actionLabel')
        self.assertEqual(self.label.text, 'No action taken.')
        self._open_composer_sheet()
        self.composer_sheet = self._select_composer_sheet()

    def _open_composer_sheet(self):
        button = self.main_view.select_single(
            'Button', objectName='openComposerSheetButton')
        self.pointing_device.click_object(button)

    def _select_composer_sheet(self):
        return self.main_view.select_single(
            emulators.ComposerSheet, objectName='testComposerSheet')

    def test_select_composer_sheet_custom_emulator(self):
        self.assertIsInstance(self.composer_sheet, emulators.ComposerSheet)

    def test_confirm_composer_sheet(self):
        self.composer_sheet.confirm()
        self.assertEqual(self.label.text, 'Confirm selected.')
        self._assert_composer_sheet_is_closed()

    def _assert_composer_sheet_is_closed(self):
        self.assertRaises(
            dbus.StateNotFoundError, self._select_composer_sheet)

    def test_cancel_composer_sheet(self):
        self.composer_sheet.cancel()
        self.assertEqual(self.label.text, 'Cancel selected.')
        self._assert_composer_sheet_is_closed()
