# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014, 2015 Canonical Ltd.
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


class TabsTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    deprecated_tabbar_test_qml_file_path = os.path.join(
        dir_path, 'test_tabs.TabsTestCase.deprecated_TabBar.qml')
    new_header_test_qml_file_path = os.path.join(
        dir_path, 'test_tabs.TabsTestCase.new_header.qml')
    new_header_1_3_test_qml_file_path = os.path.join(
        dir_path, 'test_tabs.TabsTestCase.new_header.1.3.qml')

    scenarios = [
        ('deprecated TabBar',
         dict(test_qml_file_path=deprecated_tabbar_test_qml_file_path)),
        ('new header',
         dict(test_qml_file_path=new_header_test_qml_file_path)),
        ('new header 1.3',
         dict(test_qml_file_path=new_header_1_3_test_qml_file_path))
    ]

    def test_tabs_custom_proxy_object(self):
        self.assertIsInstance(self.main_view.get_tabs(), ubuntuuitoolkit.Tabs)

    def test_get_current_tab(self):
        tabs = self.main_view.get_tabs()
        self.assertEqual(tabs.get_current_tab().title, 'Tab1')

    def test_switch_to_next_tab_from_header(self):
        header = self.main_view.get_header()
        header.switch_to_next_tab()
        current_tab = self.main_view.get_tabs().get_current_tab()
        self.assertEqual(current_tab.title, 'Tab2')

    def test_switch_to_next_tab_from_main_view(self):
        current_tab = self.main_view.switch_to_next_tab()
        self.assertEqual(current_tab.title, 'Tab2')

    def test_switch_to_next_tab_from_last(self):
        last_tab_index = self.main_view.get_tabs().get_number_of_tabs() - 1
        self.main_view.switch_to_tab_by_index(last_tab_index)
        current_tab = self.main_view.switch_to_next_tab()
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_tab_by_index(self):
        current_tab = self.main_view.switch_to_tab_by_index(2)
        self.assertEqual(current_tab.title, 'Tab3')
        current_tab = self.main_view.switch_to_tab_by_index(1)
        self.assertEqual(current_tab.title, 'Tab2')
        current_tab = self.main_view.switch_to_tab_by_index(0)
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_opened_tab_is_not_opened_again(self):
        with mock.patch.object(
                ubuntuuitoolkit.AppHeader,
                'switch_to_next_tab') as mock_switch:
            current_tab = self.main_view.switch_to_tab_by_index(0)

        self.assertFalse(mock_switch.called)
        self.assertEqual(current_tab.title, 'Tab1')

    def test_get_number_of_tabs(self):
        tabs = self.main_view.get_tabs()
        self.assertEqual(tabs.get_number_of_tabs(), 3)

    def test_swith_to_tab_by_index_out_of_range(self):
        last_tab_index = self.main_view.get_tabs().get_number_of_tabs() - 1
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            self.main_view.switch_to_tab_by_index,
            last_tab_index + 1)
        self.assertEqual(str(error), 'Tab index out of range.')

    def test_switch_to_previous_tab_from_first(self):
        current_tab = self.main_view.switch_to_previous_tab()
        self.assertEqual(current_tab.title, 'Tab3')

    def test_switch_to_previous_tab_not_from_first(self):
        self.main_view.switch_to_tab_by_index(1)
        current_tab = self.main_view.switch_to_previous_tab()
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_tab_by_object_name(self):
        current_tab = self.main_view.switch_to_tab('tab3')
        self.assertEqual(current_tab.title, 'Tab3')
        current_tab = self.main_view.switch_to_tab('tab2')
        self.assertEqual(current_tab.title, 'Tab2')
        current_tab = self.main_view.switch_to_tab('tab1')
        self.assertEqual(current_tab.title, 'Tab1')

    def test_switch_to_unexisting_tab(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.main_view.switch_to_tab,
            'unexisting')
        self.assertEqual(
            str(error), 'Tab with objectName "unexisting" not found.')
