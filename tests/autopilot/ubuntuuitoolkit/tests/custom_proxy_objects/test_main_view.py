# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2015 Canonical Ltd.
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
from unittest import mock

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class MainView10TestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_main_view.MainView10TestCase.qml')

    def test_main_view_custom_proxy_object(self):
        self.assertIsInstance(self.main_view, ubuntuuitoolkit.MainView)

    def test_get_header_without_header(self):
        header = self.main_view.get_header()
        self.assertFalse(header.visible)

    def test_toolbar_custom_proxy_object(self):
        toolbar = self.main_view.get_toolbar()
        self.assertIsInstance(toolbar, ubuntuuitoolkit.Toolbar)

    def test_open_toolbar(self):
        with mock.patch.object(ubuntuuitoolkit.Toolbar, 'open') as mock_open:
            self.main_view.open_toolbar()

        mock_open.assert_called_once_with()

    def test_close_toolbar(self):
        with mock.patch.object(ubuntuuitoolkit.Toolbar, 'close') as mock_close:
            self.main_view.close_toolbar()

        mock_close.assert_called_once_with()

    def test_open_toolbar_returns_the_toolbar(self):
        toolbar = self.main_view.open_toolbar()
        self.assertIsInstance(toolbar, ubuntuuitoolkit.Toolbar)

    def test_get_tabs_without_tabs(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.get_tabs)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_switch_to_next_tab_without_tabs(self):
        header = self.main_view.get_header()
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            header.switch_to_next_tab)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')


class MainView12TestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_main_view.MainView12TestCase.qml')

    def test_main_view_custom_proxy_object(self):
        self.assertIsInstance(self.main_view, ubuntuuitoolkit.MainView)

    def test_get_header_without_header(self):
        header = self.main_view.get_header()
        self.assertFalse(header.visible)

    def test_get_tabs_without_tabs(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.get_tabs)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_switch_to_next_tab_without_tabs(self):
        header = self.main_view.get_header()
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            header.switch_to_next_tab)
        self.assertEqual(
            str(error), 'The MainView has no Tabs.')

    def test_get_toolbar_without_toolbar(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.get_toolbar)
        self.assertEqual(
            str(error), 'The MainView has no Toolbar.')

    def test_open_toolbar_without_toolbar(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.open_toolbar)
        self.assertEqual(
            str(error), 'The MainView has no Toolbar.')

    def test_close_toolbar_without_toolbar(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.close_toolbar)
        self.assertEqual(
            str(error), 'The MainView has no Toolbar.')


class GoBackTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    toolbar_test_qml_file_path = os.path.join(
        dir_path, 'test_main_view.GoBackTestCase.back_in_toolbar.qml')
    header_test_qml_file_path = os.path.join(
        dir_path, 'test_main_view.GoBackTestCase.back_in_header.qml')

    scenarios = [
        ('back_in_toolbar', dict(
            test_qml_file_path=toolbar_test_qml_file_path)),
        ('back_in_header', dict(
            test_qml_file_path=header_test_qml_file_path))
    ]

    def setUp(self):
        super().setUp()
        self.header = self.main_view.get_header()
        self.assertEqual(self.header.title, 'Page 0')

    def test_open_page(self):
        self._go_to_page1()
        self.assertEqual(self.header.title, 'Page 1')

    def _go_to_page1(self):
        button = self.main_view.select_single(
            'Button',
            objectName='go_to_page1')
        self.pointing_device.click_object(button)
        self.header.wait_for_animation()

    def test_go_back(self):
        self._go_to_page1()
        self.main_view.go_back()
        self.assertEqual(self.header.title, 'Page 0')
