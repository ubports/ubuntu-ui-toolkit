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

from autopilot.introspection import dbus

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
        try:
            cursor = self.main_view.select_single(
                objectName='text_cursor_style_caret')
            # cursor.visible is always True if the select succeeds
            self.assertFalse(cursor.visible)
        except dbus.StateNotFoundError:
            # Caret can't be selected because it's hidden
            pass

        self.pointing_device.click_object(self.textfield)
        self.assertTrue(self.textfield.focus)
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret')
        self.assertTrue(cursor.visible)

    def test_caret_hide_while_typing(self):
        self.pointing_device.click_object(self.textfield)
        self.assertTrue(self.textfield.focus)
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret')
        self.assertTrue(cursor.visible)

        self.textfield.keyboard.type('Lorem ipsum')
        self.assertFalse(cursor.visible)

    def test_caret_visible_after_tapping(self):
        self.test_caret_hide_while_typing()
        self.pointing_device.click_object(self.textfield)
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret')
        self.assertTrue(cursor.visible)

    def test_caret_visible_after_selecting(self):
        self.test_caret_hide_while_typing()
        # Select a character
        self.keyboard.press_and_release('Shift+Left')
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret_end')
        self.assertTrue(cursor.visible)
