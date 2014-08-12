# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery - TextInput components"""

import testscenarios

from autopilot.introspection import dbus
from ubuntuuitoolkit import emulators, ubuntu_scenarios
from ubuntuuitoolkit.tests.gallery import GalleryTestCase
import locale


class WriteAndClearTextInputTestCase(GalleryTestCase):

    def text_to_write_string():
        return 'Hello World'

    def text_to_write_number():
        return locale.format('%.2f', -1001.23)

    # text_to_write is a function to ensure
    # that locale is evaluated after setUp
    text_input_scenarios = [
        ('standard textfield', dict(
            objectName='textfield_standard',
            text_to_write=text_to_write_string,
            expected_text=text_to_write_string())),
        ('password textfield', dict(
            objectName='textfield_password',
            text_to_write=text_to_write_string,
            expected_text=text_to_write_string())),
        # The text_to_write contains a decimal separator based on locale
        # eg. -1001.23 or -1001,23 or -۱۰۰۱٫۲۳
        # The test expects integers, TextField rejects that character
        ('only integers textfield', dict(
            objectName='textfield_numbers',
            text_to_write=text_to_write_number,
            expected_text='-100123'))
    ]

    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        text_input_scenarios)

    def setUp(self):
        super(WriteAndClearTextInputTestCase, self).setUp()
        # Apply the user locale from the environment
        # The UITK does the same, so the test must be localized
        locale.setlocale(locale.LC_ALL, "")
        self.open_page('textinputsElement')

    def test_write_on_textfield_must_update_text(self):
        textfield = self.main_view.select_single(
            emulators.TextField, objectName=self.objectName)

        textfield.write(self.text_to_write())
        self.assertEqual(self.expected_text, textfield.text)

    def test_clear_textfield_must_remove_text(self):
        textfield = self.main_view.select_single(
            emulators.TextField, objectName=self.objectName)
        textfield.write(self.text_to_write())

        textfield.clear()
        self.assertEqual('', textfield.text)


class DisabledTextInputTestCase(GalleryTestCase):

    scenarios = ubuntu_scenarios.get_device_simulation_scenarios()

    def setUp(self):
        super(DisabledTextInputTestCase, self).setUp()
        self.open_page('textinputsElement')

    def test_textfield_disabled(self):
        textfield_disabled = self.main_view.select_single(
            emulators.TextField, objectName='textfield_disabled')
        self.assertFalse(textfield_disabled.enabled)

        self.pointing_device.click_object(textfield_disabled)
        textfield_disabled.keyboard.type('This should not be writen')
        self.assertEqual('', textfield_disabled.text)


class CaretTextInputTestCase(GalleryTestCase):

    scenarios = [
        ('textfield', dict(objectName='textfield_standard')),
        ('textarea', dict(objectName='textarea_default')),
    ]

    def get_command_line(self, command_line):
        command_line.append('-touch')
        return command_line

    def setUp(self):
        super(CaretTextInputTestCase, self).setUp()
        self.open_page('textinputsElement')
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
        self.textfield._select_all()
        cursor = self.main_view.select_single(
            objectName='text_cursor_style_caret_end')
        self.assertTrue(cursor.visible)
