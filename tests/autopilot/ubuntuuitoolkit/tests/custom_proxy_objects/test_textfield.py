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

import os
from unittest import mock

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class TextFieldTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    qml_file_path_1_0 = os.path.join(
        dir_path, 'test_textfield.1.0.qml')
    qml_file_path_1_3 = os.path.join(
        dir_path, 'test_textfield.1.3.qml')
    scenarios = [
        ('1.0',
            dict(test_qml_file_path=qml_file_path_1_0)),
        ('1.3',
            dict(test_qml_file_path=qml_file_path_1_3)),
    ]

    def setUp(self):
        super().setUp()
        self.simple_text_field = self.main_view.select_single(
            ubuntuuitoolkit.TextField, objectName='simple_text_field')

    def test_text_field_custom_proxy_object(self):
        self.assertIsInstance(
            self.simple_text_field, ubuntuuitoolkit.TextField)

    def test_write(self):
        self.simple_text_field.write('test')
        self.assertEqual(self.simple_text_field.text, 'test')

    def test_clear_with_clear_button(self):
        self.simple_text_field.write('test')
        self.simple_text_field.clear()
        self.assertEqual(self.simple_text_field.text, '')

    def test_clear_without_clear_button(self):
        text_field = self.main_view.select_single(
            ubuntuuitoolkit.TextField,
            objectName='text_field_without_clear_button')
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

    def test_select_all_selects_all_text(self):
        self.simple_text_field.write('Text to select.')
        self.simple_text_field._select_all()

        self.assertTrue(self.simple_text_field._is_all_text_selected())

    def test_select_all_when_already_selected_must_do_nothing(self):
        """Test for select all the text when it's already selected."""
        self.simple_text_field.write('Text to select.')
        self.simple_text_field._select_all()
        with mock.patch.object(
                self.simple_text_field, 'pointing_device') as mock_device:
            self.simple_text_field._select_all()

        self.assertFalse(mock_device.called)
