# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Header component."""

import os
import testtools
from time import sleep

from autopilot.input._common import get_center_point
from autopilot import platform

from ubuntuuitoolkit import tests


class CaretTextInputTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    textfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield.qml')
    textarea_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textarea.qml')
    customfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield_custom.qml')

    scenarios = [
        ('textfield',
            dict(test_qml_file_path=textfield_qml_file_path,
                 objectName='textfield')),
        ('textarea',
            dict(test_qml_file_path=textarea_qml_file_path,
                 objectName='textarea')),
        ('customfield',
            dict(test_qml_file_path=customfield_qml_file_path,
                 objectName='textfield')),
    ]

    def get_command_line(self, command_line):
        command_line.append('-touch')
        return command_line

    def setUp(self):
        super(CaretTextInputTestCase, self).setUp()
        self.textfield = self.main_view.select_single(
            objectName=self.objectName)
        self.assertFalse(self.textfield.focus)

    def test_caret_visible_on_focus(self):
        cursorName = 'text_cursor_style_caret_cursorPosition'
        self._assert_not_visible(objectName=cursorName)
        self.pointing_device.click_object(self.textfield)
        self.assertTrue(self.textfield.focus)
        cursor = self.main_view.select_single(objectName=cursorName)
        self.assertTrue(cursor.visible)

    def test_caret_hide_while_typing(self):
        self.pointing_device.click_object(self.textfield)
        self.assertTrue(self.textfield.focus)
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret_cursorPosition')
        self.assertTrue(cursor.visible)

        self.textfield.keyboard.type('Lorem ipsum')
        self.assertFalse(cursor.visible)

    def test_caret_visible_after_tapping(self):
        self.test_caret_hide_while_typing()
        self.pointing_device.click_object(self.textfield)
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret_cursorPosition')
        self.assertTrue(cursor.visible)

    def test_caret_visible_after_selecting(self):
        self.test_caret_hide_while_typing()
        # Select a character
        self.keyboard.press_and_release('Shift+Left')
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret_selectionEnd')
        self.assertTrue(cursor.visible)


class InsertModeTextInputTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    textfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield.qml')
    textarea_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textarea.qml')
    customfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield_custom.qml')

    scenarios = [
        ('textfield',
            dict(test_qml_file_path=textfield_qml_file_path,
                 objectName='textfield')),
        ('textarea',
            dict(test_qml_file_path=textarea_qml_file_path,
                 objectName='textarea')),
        ('customfield',
            dict(test_qml_file_path=customfield_qml_file_path,
                 objectName='textfield')),
    ]

    def get_command_line(self, command_line):
        command_line.append('-touch')
        return command_line

    def setUp(self):
        super(InsertModeTextInputTestCase, self).setUp()
        self.textfield = self.main_view.select_single(
            objectName=self.objectName)
        self.assertFalse(self.textfield.focus)

    def assert_buttons(self, texts):
        popover = self.main_view.get_text_input_context_menu(
            'text_input_contextmenu')
        for text in texts:
            button = popover._get_button(text)
            self.assertTrue(button.visible)

    def assert_discard_popover(self):
        # Discard popover by tap
        self.pointing_device.move(
            self.textfield.globalRect.x + self.textfield.width * 0.7,
            self.textfield.globalRect.y + self.textfield.height // 10)
        self.pointing_device.click()

        self._assert_not_visible(objectName='text_input_contextmenu')

    def test_popover_visible_after_tapping_caret(self):
        # Insert Mode
        self.pointing_device.click_object(self.textfield)
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_cursorPosition')
        self.pointing_device.click_object(cursor)
        self.assert_buttons(['Select All', 'Paste'])
        self.assert_discard_popover()

    def test_popover_visible_after_dragging_caret(self):
        # Insert Mode
        self.pointing_device.click_object(self.textfield)
        self.textfield.keyboard.type('Lorem ipsum')
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_cursorPosition')
        x, y = get_center_point(cursor)
        self.pointing_device.drag(x, y, x + self.textfield.width // 2, y)
        self.assert_buttons(['Select All', 'Paste'])
        self.assert_discard_popover()

    @testtools.skipIf(platform.model() == 'Desktop', 'Touch only')
    def test_popover_visible_after_long_press(self):
        # Select Mode
        self.pointing_device.click_object(self.textfield)
        self.textfield.keyboard.type('Lorem ipsum')
        self.pointing_device.move(
            self.textfield.globalRect.x + self.textfield.width // 8,
            self.textfield.globalRect.y + self.textfield.height // 2)
        # Long press to select a word
        # FIXME: input.Mouse doesn't support long press
        # press_duration doesn't work here
        # self.pointing_device.click(press_duration=2.0)
        self.pointing_device.press()
        sleep(2)
        self.pointing_device.release()
        self.assert_buttons(['Cut', 'Copy', 'Paste'])
        self.assert_discard_popover()
