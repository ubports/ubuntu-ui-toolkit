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


class ActionBarTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    overflow_test_qml_file_path = os.path.join(
        dir_path, 'test_actionbar.ActionBarTestCase.qml')
    scrolling_test_qml_file_path = os.path.join(
        dir_path, 'test_actionbar.ScrollingActionBarTestCase.qml')

    scenarios = [
        ('overflow', dict(test_qml_file_path=overflow_test_qml_file_path)),
        ('scrolling', dict(test_qml_file_path=scrolling_test_qml_file_path))
    ]

    def setUp(self):
        super().setUp()
        self.actionbar = self.app.select_single(
            'ActionBar', objectName='ActionBar')
        self.label = self.app.select_single(objectName='Label')
        self.assertEqual(self.label.text, 'No action triggered.')

    def test_custom_proxy_object(self):
        self.assertIsInstance(self.actionbar, ubuntuuitoolkit.ActionBar)
        self.assertTrue(self.actionbar.visible)

    def test_click_action_button(self):
        for i in range(1, 4):
            self.actionbar.click_action_button('Action' + str(i))
            self.assertEqual(self.label.text, 'Action ' + str(i) +
                             ' triggered.')

    def test_click_overflow_action_button(self):
        # Action1-3 are directly the ActionBar, Action4-5 are in the overflow
        for i in range(4, 6):
            self.actionbar.click_action_button('Action' + str(i))
            self.assertEqual(self.label.text, 'Action ' + str(i) +
                             ' triggered.')

    def test_click_unexisting_action_button(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            self.actionbar.click_action_button, 'unexisting')
        self.assertEqual(
            str(error),
            'Button not found')
