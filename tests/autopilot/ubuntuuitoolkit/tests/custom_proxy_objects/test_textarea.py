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

    def test_text_area_inherits_text_field(self):
        self.assertTrue(
            issubclass(ubuntuuitoolkit.TextArea, ubuntuuitoolkit.TextField))

    def test_clear(self):
        self.simple_text_area.write('test')
        self.simple_text_area.clear()
        self.assertEqual(self.simple_text_area.text, '')
