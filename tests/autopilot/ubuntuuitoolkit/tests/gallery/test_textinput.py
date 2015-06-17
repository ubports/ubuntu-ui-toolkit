# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014, 2015 Canonical Ltd.
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
        super().setUp()
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
        super().setUp()
        self.open_page('textinputsElement')

    def test_textfield_disabled(self):
        textfield_disabled = self.main_view.select_single(
            emulators.TextField, objectName='textfield_disabled')
        self.assertFalse(textfield_disabled.enabled)

        disabled_text = textfield_disabled.text
        self.assertRaises(AssertionError,
                          textfield_disabled.write,
                          'This should not be written')
        self.assertEqual(disabled_text, textfield_disabled.text)
