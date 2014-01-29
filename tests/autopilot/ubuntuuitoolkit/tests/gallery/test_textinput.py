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

from ubuntuuitoolkit import emulators
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class WriteAndClearTextInputTestCase(GalleryTestCase):

    scenarios = [
        ('standard textfield', dict(
            objectName='textfield_standard', text_to_write='Hello World',
            expected_text='Hello World')),
        ('password textfield', dict(
            objectName='textfield_password', text_to_write='Test password',
            expected_text='Test password')),
        ('only integers textfield', dict(
            objectName='textfield_numbers', text_to_write='-100.123',
            expected_text='-100123'))
    ]

    def setUp(self):
        super(WriteAndClearTextInputTestCase, self).setUp()
        item = 'Text Field'
        self.loadItem(item)
        self.checkPageHeader(item)

    def test_write_on_textfield_must_update_text(self):
        textfield = self.main_view.select_single(
            emulators.TextField, objectName=self.objectName)

        textfield.write(self.text_to_write)
        self.assertEqual(self.expected_text, textfield.text)

    def test_clear_textfield_must_remove_text(self):
        textfield = self.main_view.select_single(
            emulators.TextField, objectName=self.objectName)
        textfield.write(self.text_to_write)

        textfield.clear()
        self.assertEqual('', textfield.text)


class DisabledTextInputTestCase(GalleryTestCase):

    def setUp(self):
        super(DisabledTextInputTestCase, self).setUp()
        item = 'Text Field'
        self.loadItem(item)
        self.checkPageHeader(item)

    def test_textfield_disabled(self):
        textfield_disabled = self.main_view.select_single(
            emulators.TextField, objectName='textfield_disabled')
        self.assertFalse(textfield_disabled.enabled)

        textfield_disabled.write('This should not be writen')
        self.assertEqual('', textfield_disabled.text)
