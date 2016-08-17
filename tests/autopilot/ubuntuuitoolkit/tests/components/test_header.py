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

"""Tests for the Ubuntu UI Toolkit Header component."""

import os

from autopilot.introspection import dbus

from ubuntuuitoolkit import tests


class HeaderActionsOverflowTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    tools_test_qml_file_path = os.path.join(
        dir_path, 'test_header.HeaderActionsOverflowTestCase.tools.qml')
    actions_test_qml_file_path = os.path.join(
        dir_path, 'test_header.HeaderActionsOverflowTestCase.actions.qml')

    scenarios = [
        ('deprecated tools',
            dict(test_qml_file_path=tools_test_qml_file_path)),
        ('actions',
            dict(test_qml_file_path=actions_test_qml_file_path))
    ]

    def setUp(self):
        super().setUp()
        self.header = self.main_view.get_header()

    def test_click_header_overflow_action_must_close_popover(self):
        """Test that clicking an action from the popover must close it.

        Regression test for http://pad.lv/1326963

        """
        self.header.click_action_button('pushStackAction')
        # the popover was created to click the action button
        # popover will be closed and destroyed immediately after
        # clicking the action button
        try:
            overflow_popover = self.main_view.select_single(
                objectName='actions_overflow_popover')
            overflow_popover.wait_until_destroyed()
        except dbus.StateNotFoundError:
            # overflow popover was already destroyed before
            # it could be selected
            pass


class HeaderContentsTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_header.HeaderContentsTestCase.qml')

    def setUp(self):
        super().setUp()
        self.header = self.main_view.get_header()

    def test_open_page_with_head_contents_must_change_header(self):
        """Test that setting head.contents overrides header contents."""

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
        self.header.wait_for_animation()

        self.assertEqual(label.visible, False)
        headerContents = self.header.select_single(
            objectName='orange_header_contents')
        self.assertEqual(headerContents.visible, True)

    def test_close_page_with_head_contents_must_revert_header(self):
        """Test that closing a page with head.contents reverts the header."""
        pushButton = self.main_view.select_single(
            'Button',
            objectName='push_button')
        self.pointing_device.move_to_object(pushButton)
        self.pointing_device.click()

        self.header.wait_for_animation()
        headerContents = self.header.select_single(
            objectName='orange_header_contents')
        label = self.header.select_single(
            'Label',
            objectName='header_title_label')

        self.assertEqual(headerContents.visible, True)
        self.assertEqual(label.visible, False)

        self.header.click_back_button()
        self.assertEqual(headerContents.visible, False)
        self.assertEqual(label.visible, True)
        self.assertEqual(label.text, 'Test title')
