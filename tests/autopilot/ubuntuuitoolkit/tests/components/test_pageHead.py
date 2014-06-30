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

"""Tests for the Ubuntu UI Toolkit Header component."""

import os

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntuuitoolkit import tests


class HeaderTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_header.HeaderTestCase.qml')
    test_qml_file_path = os.path.join(
        dir_path, 'test_pageHead.qml')
    actions_test_qml_file_path = os.path.join(
        dir_path, 'test_header.HeaderActionsTestCase.qml')

    def setUp(self):
        super(HeaderTestCase, self).setUp()
        self.header = self.main_view.get_header()

    def test_contents(self):
        """Test that setting head.contents overrides header contents.

        """
        label = self.header.select_single(
            'Label',
            objectName='header_title_label')
        self.assertEqual(label.visible, True)
        self.assertEqual(label.text, 'Test title')

        pushButton = self.main_view.select_single(
            'Button',
            objectName='push_button')
        self.pointing_device.move_to_object(pushButton)
        self.pointing_device.click()

        self.assertEqual(label.visible, False)
        headerContents = self.header.select_single(
            objectName='orange_header_contents'
        )
        self.assertEqual(headerContents.visible, True)

        self.header.click_back_button()
        self.assertEqual(headerContents.visible, False)
        self.assertEqual(label.visible, True)
        self.assertEqual(label.text, 'Test title')
