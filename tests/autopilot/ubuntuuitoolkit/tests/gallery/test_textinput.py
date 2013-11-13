# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
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
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class TextInputTests(GalleryTestCase):
    """Generic tests for the Gallery"""

    def test_textfield_standard(self):
        item = "Text Field"
        self.loadItem(item)
        self.checkPageHeader(item)
        textfield_standard = self.getObject('textfield_standard')
        self.pointing_device.click_object(textfield_standard)
        self.assertThat(textfield_standard.focus, Eventually(Equals(True)))
        self.type_string('Hello World')
        self.assertThat(textfield_standard.text,
                        Eventually(Equals('Hello World')))

    def test_textfield_password(self):
        item = "Text Field"
        self.loadItem(item)
        self.checkPageHeader(item)
        textfield_password = self.getObject('textfield_password')
        self.pointing_device.click_object(textfield_password)
        self.assertThat(textfield_password.text, Equals('password'))

        self.tap_clearButton('textfield_password')
        self.assertThat(textfield_password.text, Eventually(Equals('')))

        self.type_string('abcdefgh123')
        self.assertThat(textfield_password.text,
                        Eventually(Equals('abcdefgh123')))

    def test_textfield_numbers(self):
        item = "Text Field"
        self.loadItem(item)
        self.checkPageHeader(item)
        textfield_numbers = self.getObject('textfield_numbers')
        self.assertThat(textfield_numbers.text, Eventually(Equals('123')))

        self.tap_clearButton('textfield_numbers')
        self.assertEqual(textfield_numbers.text, '')
        #try typing decimal value when text filed is int only.
        self.type_string('-100.123')
        self.assertThat(textfield_numbers.text, Eventually(Equals('-100123')))

    def test_textfield_disabled(self):
        item = "Text Field"
        self.loadItem(item)
        self.checkPageHeader(item)
        textfield_disabled = self.getObject('textfield_disabled')
        self.assertFalse(textfield_disabled.enabled)
        #try tapping a disabled field and verify that focus is false.
        self.pointing_device.click_object(textfield_disabled)
        self.assertFalse(textfield_disabled.focus)
