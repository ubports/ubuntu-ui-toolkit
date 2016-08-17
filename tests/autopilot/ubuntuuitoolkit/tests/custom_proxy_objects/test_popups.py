# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2015 Canonical Ltd.
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

from autopilot.introspection import dbus

import os
import ubuntuuitoolkit
from ubuntuuitoolkit import popups, tests


class ActionSelectionPopoverTestCase(tests.QMLFileAppTestCase):

    # FIXME: Currently we are only testing UITK 1.0 Popups,
    #   we need to test 1.3 as well. See bug 1612582.
    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_popups.ActionSelectionPopoverTestCase.qml')

    def test_custom_proxy_object(self):
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        self.assertIsInstance(popover, popups.ActionSelectionPopover)

    def test_click_button_by_label(self):
        label = self.app.select_single('Label', objectName='clicked_label')
        self.assertNotEqual(label.text, 'Button clicked.')
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        popover.click_button_by_text('Action one')
        self.assertEqual(label.text, 'Button clicked.')

    def test_click_button_by_object_name(self):
        label = self.app.select_single('Label', objectName='clicked_label')
        self.assertNotEqual(label.text, 'Button clicked.')
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        popover.click_action_button('actionOne')
        self.assertEqual(label.text, 'Button clicked.')

    def test_click_unexisting_button_by_object_name(self):
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            popover.click_action_button, 'actionTwo')
        self.assertEqual(
            str(error),
            'Button for action with objectName "actionTwo" not found.')

    def test_click_disabled_button_by_object_name(self):
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        # Disabled actions are not shown in ActionSelectionPopover
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            popover.click_action_button, 'actionDisabled')
        self.assertEqual(
            str(error),
            'Button for action with objectName "actionDisabled" not visible.')

    def test_click_hidden_button_by_object_name(self):
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            popover.click_action_button, 'actionHidden')
        self.assertEqual(
            str(error),
            'Button for action with objectName "actionHidden" not visible.')

    def _open_popover(self):
        open_button = self.main_view.select_single(
            'Button', objectName='open_popover')
        self.pointing_device.click_object(open_button)

    def test_click_unexisting_button(self):
        self._open_popover()
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, popover.click_button_by_text,
            'unexisting')
        self.assertEqual(
            str(error), 'Button with text "unexisting" not found.')

    def test_click_button_with_closed_popover(self):
        popover = self.main_view.get_action_selection_popover(
            'test_actions_popover')
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, popover.click_button_by_text,
            'Action one')
        self.assertEqual(
            str(error), 'The popover is not open.')


class ComposerSheetTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_popups.ComposerSheetTestCase.qml')

    def setUp(self):
        super().setUp()
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
            popups.ComposerSheet, objectName='testComposerSheet')

    def test_select_composer_sheet_custom_proxy_object(self):
        self.assertIsInstance(
            self.composer_sheet, popups.ComposerSheet)

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
