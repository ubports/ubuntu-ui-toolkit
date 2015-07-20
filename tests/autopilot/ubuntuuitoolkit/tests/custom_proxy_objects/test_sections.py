# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2015 Canonical Ltd.
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
import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class SectionsTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_sections.SectionsTestCase.qml')

    def setUp(self):
        super().setUp()
        self.sections = self.app.select_single(
            ubuntuuitoolkit.Sections, objectName='sections')
        self.label = self.app.select_single(
            'Label', objectName='label')
        self.assertEqual(self.label.text, 'Section 0 is selected.')

    def test_custom_proxy_object(self):
        self.assertIsInstance(self.sections, ubuntuuitoolkit.Sections)
        self.assertTrue(self.sections.visible)

    def test_click_section_button(self):
        self.sections.click_section_button(2)
        self.assertEqual(self.label.text, 'Section 2 is selected.')

    def test_click_unexisting_section_button(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            self.sections.click_section_button, 3)
        self.assertEqual(
            str(error),
            'Button not found in Sections.')
