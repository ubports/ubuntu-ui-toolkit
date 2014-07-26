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

try:
    from unittest import mock
except ImportError:
    import mock

from autopilot import platform

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class TextAreaTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Item {
        TextArea {
            objectName: "simple_text_area"
        }
    }
}
""")

    def setUp(self):
        super(TextAreaTestCase, self).setUp()
        self.simple_text_area = self.main_view.select_single(
            ubuntuuitoolkit.TextArea, objectName='simple_text_area')

    def test_text_area_custom_proxy_object(self):
        self.assertIsInstance(
            self.simple_text_area, ubuntuuitoolkit.TextArea)

    def test_write(self):
        self.simple_text_area.write('test')
        self.assertEqual(self.simple_text_area.text, 'test')

    def test_clear(self):
        self.simple_text_area.write('test')
        self.simple_text_area.clear()
        self.assertEqual(self.simple_text_area.text, '')

    def test_clear_and_write(self):
        self.simple_text_area.write('test1')
        self.simple_text_area.write('test2')
        self.assertEqual(self.simple_text_area.text, 'test2')

    def test_write_without_clear(self):
        self.simple_text_area.write('test1')
        self.simple_text_area.write('test2', clear=False)
        self.assertEqual(self.simple_text_area.text, 'test1test2')

    def test_write_without_clear_writes_at_the_end(self):
        self.simple_text_area.write(
            'long text that will fill more than half of the text area.')
        self.simple_text_area.write('append', clear=False)
        self.assertEqual(
            self.simple_text_area.text,
            'long text that will fill more than half of the text area.append')

    def test_is_empty(self):
        self.assertTrue(self.simple_text_area.is_empty())
        self.simple_text_area.write('test')
        self.assertFalse(self.simple_text_area.is_empty())

    def test_select_all_selects_all_text(self):
        if platform.model() != 'Desktop':
            self.skipTest('Select all is not yet implemented on the phone.')
        self.simple_text_area.write('Text to select.')
        self.simple_text_area._select_all()

        self.assertTrue(self.simple_text_area._is_all_text_selected())

    def test_select_all_when_already_selected_must_do_nothing(self):
        """Test for select all the text when it's already selected."""
        if platform.model() != 'Desktop':
            self.skipTest('Select all is not yet implemented on the phone.')
        self.simple_text_area.write('Text to select.')
        self.simple_text_area._select_all()
        with mock.patch.object(
                self.simple_text_area, 'pointing_device') as mock_device:
            self.simple_text_area._select_all()

        self.assertFalse(mock_device.called)
