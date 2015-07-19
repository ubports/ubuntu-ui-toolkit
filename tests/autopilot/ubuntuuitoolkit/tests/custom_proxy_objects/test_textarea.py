# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014-2015 Canonical Ltd.
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

from testtools.matchers import GreaterThan

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class TextAreaTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Item {
        TextArea {
            objectName: "simple_text_area"
        }
    }
}
""")

    def setUp(self):
        super().setUp()
        self.simple_text_area = self.main_view.select_single(
            ubuntuuitoolkit.TextArea, objectName='simple_text_area')

    def test_text_area_inherits_text_field(self):
        self.assertTrue(
            issubclass(ubuntuuitoolkit.TextArea, ubuntuuitoolkit.TextField))

    def test_clear_with_single_line(self):
        self.simple_text_area.write('test')
        self.simple_text_area.clear()
        self.assertEqual(self.simple_text_area.text, '')

    def test_clear_with_multiple_lines(self):
        # This is a regrestion test for http://pad.lv/1359167
        self.simple_text_area.write(
            'Long text that will make it wrap into multiple lines.')
        self.assertThat(self.simple_text_area.lineCount, GreaterThan(1))
        self.simple_text_area._go_to_start()
        self.simple_text_area.clear()
        self.assertEqual(self.simple_text_area.text, '')
