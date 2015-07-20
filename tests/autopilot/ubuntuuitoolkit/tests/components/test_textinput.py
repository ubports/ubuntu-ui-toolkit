# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014, 2015 Canonical Ltd.
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

from ubuntuuitoolkit import (
    TextArea,
    TextField,
    tests
)


class CaretTextInputTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    textfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield.qml')
    textarea_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textarea.qml')
    customfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield_custom.qml')
    header_qml_file_path = os.path.join(
        dir_path, 'test_textinput.header.qml')

    scenarios = [
        ('textfield',
            dict(test_qml_file_path=textfield_qml_file_path,
                 objectName='textfield',
                 cpo_class=TextField)),
        ('textarea',
            dict(test_qml_file_path=textarea_qml_file_path,
                 objectName='textarea',
                 cpo_class=TextArea)),
        ('customfield',
            dict(test_qml_file_path=customfield_qml_file_path,
                 objectName='textfield',
                 cpo_class=TextField)),
        ('header',
            dict(test_qml_file_path=header_qml_file_path,
                 objectName='textfield',
                 cpo_class=TextField)),
    ]

    def get_command_line(self, command_line):
        command_line.append('-touch')
        return command_line

    def setUp(self):
        super().setUp()
        # Create either a TextField or TextArea.
        self.textfield = self.cpo_class.from_proxy_object(
            self.main_view.select_single(objectName=self.objectName)
        )
        self.assertFalse(self.textfield.focus)

    def select_cursor(self, positionProperty):
        # The cursor may not receive events right away
        sleep(1)
        return self.main_view.select_single(
            objectName=positionProperty + '_draggeditem')

    def test_caret_hidden_if_empty(self):
        cursorName = 'text_cursor_style_caret_cursorPosition'
        self._assert_not_visible(objectName=cursorName)
        self.pointing_device.click_object(self.textfield)
        self.assertTrue(self.textfield.focus)
        self._assert_not_visible(objectName=cursorName)

    def test_caret_visible_on_focus(self):
        cursorName = 'text_cursor_style_caret_cursorPosition'
        self._assert_not_visible(objectName=cursorName)
        self.textfield.write('Lorem ipsum')
        self.pointing_device.click_object(self.textfield)
        self.main_view.select_single(objectName=cursorName)

    def test_caret_hide_while_typing(self):
        self.pointing_device.click_object(self.textfield)
        self.assertTrue(self.textfield.focus)
        cursor = self.select_cursor('cursorPosition')

        self.textfield.write('Lorem ipsum')
        self.assertFalse(cursor.visible)

    def test_caret_visible_after_tapping(self):
        self.test_caret_hide_while_typing()
        self.pointing_device.click_object(self.textfield)
        self.select_cursor('cursorPosition')

    @testtools.skipUnless(platform.model() == 'Desktop',
                          'Desktop only due to non-OSK key used.')
    def test_caret_visible_after_selecting(self):
        self.test_caret_hide_while_typing()
        # Select a character
        self.keyboard.press_and_release('Shift+Left')
        self.select_cursor('selectionEnd')


class InsertModeTextInputTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    textfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield.qml')
    textarea_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textarea.qml')
    customfield_qml_file_path = os.path.join(
        dir_path, 'test_textinput.textfield_custom.qml')
    header_qml_file_path = os.path.join(
        dir_path, 'test_textinput.header.qml')

    scenarios = [
        ('textfield',
         dict(test_qml_file_path=textfield_qml_file_path,
              objectName='textfield',
              cpo_class=TextField)),
        ('textarea',
         dict(test_qml_file_path=textarea_qml_file_path,
              objectName='textarea',
              cpo_class=TextField)),
        ('customfield',
         dict(test_qml_file_path=customfield_qml_file_path,
              objectName='textfield',
              cpo_class=TextField)),
        ('header',
         dict(test_qml_file_path=header_qml_file_path,
              objectName='textfield',
              cpo_class=TextField)),
    ]

    def get_command_line(self, command_line):
        command_line.append('-touch')
        return command_line

    def setUp(self):

        super().setUp()
        # textfield will either be a textfield or textarea.
        self.textfield = self.cpo_class.from_proxy_object(
            self.main_view.select_single(objectName=self.objectName)
        )
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
            self.textfield.globalRect.x + self.textfield.width * 0.6,
            self.textfield.globalRect.y + self.textfield.height * 0.95)
        self.pointing_device.click()

        self._assert_not_visible(objectName='text_input_contextmenu')

    def select_cursor(self, positionProperty):
        # The cursor may not receive events right away
        sleep(1)
        return self.main_view.select_single(
            objectName=positionProperty + '_draggeditem')

    def test_popover_not_obscured(self):
        self.textfield.write('Lorem ipsum')
        self.pointing_device.click_object(self.textfield)
        cursor = self.select_cursor('cursorPosition')
        self.pointing_device.click_object(cursor)
        popover = self.main_view.get_text_input_context_menu(
            'text_input_contextmenu')
        self.assertTrue(popover.globalRect.y > 0,
                        '%s <= 0' % popover.globalRect.y)

    def test_header_undisturbed_by_text_handlers(self):
        # Verify that handlers aren't accidentally placed at absolute 0/0
        self.pointing_device.click_object(self.textfield)
        # Back will hide when pressed
        back = self.main_view.select_single(objectName='customBackButton')
        self.main_view.get_header().click_custom_back_button()
        self.assertFalse(back.visible)

    def test_popover_visible_after_tapping_caret(self):
        # Insert Mode
        self.textfield.write('Lorem ipsum')
        self.pointing_device.click_object(self.textfield)
        cursor = self.select_cursor('cursorPosition')
        self.pointing_device.click_object(cursor)
        self.assert_buttons(['Select All', 'Paste'])
        self.assert_discard_popover()

    def test_popover_visible_after_dragging_caret(self):
        # Insert Mode
        self.textfield.write('Lorem ipsum')
        self.pointing_device.click_object(self.textfield)
        cursor = self.select_cursor('cursorPosition')
        x, y = get_center_point(cursor)
        self.pointing_device.drag(x, y, x + self.textfield.width // 2, y)
        self.assert_buttons(['Select All', 'Paste'])
        self.assert_discard_popover()

    @testtools.skipIf(platform.model() == 'Desktop', 'Touch only')
    def test_popover_visible_after_long_press(self):
        # Select Mode
        self.textfield.write('Lorem ipsum')
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
