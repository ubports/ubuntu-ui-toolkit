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

import fixtures
#from testtools.matchers import Contains
#from autopilot import introspection

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class ActionBarTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_actionbar.ActionBarTestCase.qml')

    def setUp(self):
        super().setUp()
        self.bar = self.app.select_single(
            'ActionBar', objectName='ActionBar')
        self.label = self.app.select_single(
            'Label', objectName='Label')
        self.assertEqual(self.label.text, 'No action triggered.')

    def test_custom_proxy_object(self):
        self.assertIsInstance(self.bar, ubuntuuitoolkit.ActionBar)
        self.assertTrue(self.bar.visible)

#    def test_click_action_button(self):
#        self.header.click_action_button('action0')
#        self.assertEqual(self.label.text, 'Button 0 clicked.')

#    def test_click_overflow_action_button(self):
#        # custom back button and first action button go in the header
#        # and the others in the overflow.
#        self.header.click_action_button('action3')
#        self.assertEqual(self.label.text, 'Button 3 clicked.')

#    def test_click_unexisting_action_button(self):
#        error = self.assertRaises(
#            ubuntuuitoolkit.ToolkitException, self.header.click_action_button,
#            'unexisting')
#        self.assertEqual(
#            str(error),
#            'Button not found in header or overflow')
