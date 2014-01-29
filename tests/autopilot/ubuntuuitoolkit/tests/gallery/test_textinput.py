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

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntuuitoolkit import emulators
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class TextInputTestCase(GalleryTestCase):
    """Generic tests for the Gallery"""

    def setUp(self):
        super(TextInputTestCase, self).setUp()
        item = 'Text Field'
        self.loadItem(item)
        self.checkPageHeader(item)

    def test_textfield_standard(self):
        textfield_standard = self.main_view.select_single(
            emulators.TextField, objectName='textfield_standard')
        self.pointing_device.click_object(textfield_standard)
        self.assertThat(textfield_standard.focus, Eventually(Equals(True)))
        textfield_standard.write('Hello World')
        self.assertThat(textfield_standard.text,
                        Eventually(Equals('Hello World')))

    def test_textfield_password(self):
        textfield_password = self.main_view.select_single(
            emulators.TextField, objectName='textfield_password')
        self.pointing_device.click_object(textfield_password)
        self.assertThat(textfield_password.text, Equals('password'))

        self.tap_clearButton('textfield_password')
        self.assertThat(textfield_password.text, Eventually(Equals('')))

        textfield_password.write('abcdefgh123')
        self.assertThat(textfield_password.text,
                        Eventually(Equals('abcdefgh123')))

    def test_textfield_numbers(self):
        textfield_numbers = self.main_view.select_single(
            emulators.TextField, objectName='textfield_numbers')
        self.assertThat(textfield_numbers.text, Eventually(Equals('123')))

        textfield_numbers.clear()
        self.assertThat(textfield_numbers.text, Eventually(Equals('')))
        #try typing decimal value when text filed is int only.
        textfield_numbers.write('-100.123')
        self.assertThat(textfield_numbers.text, Eventually(Equals('-100123')))

    def test_textfield_disabled(self):
        textfield_disabled = self.main_view.select_single(
            emulators.TextField, objectName='textfield_disabled')
        self.assertFalse(textfield_disabled.enabled)
        #try tapping a disabled field and verify that focus is false.
        self.pointing_device.click_object(textfield_disabled)
        self.assertFalse(textfield_disabled.focus)
