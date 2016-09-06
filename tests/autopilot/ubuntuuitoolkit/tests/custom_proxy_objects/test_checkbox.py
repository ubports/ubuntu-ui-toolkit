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

import time
import os

from autopilot import input
from testtools.matchers import GreaterThan, LessThan
from unittest import mock

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class ToggleTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    switch_test_qml_file_path = os.path.join(
        dir_path, 'test_checkbox.ToggleTestCase.test_with_switch.qml')
    checkbox_test_qml_file_path = os.path.join(
        dir_path, 'test_checkbox.ToggleTestCase.test_with_checkbox.qml')

    scenarios = [
        ('checkbox', dict(
            test_qml_file_path=checkbox_test_qml_file_path,
            objectName='test_checkbox')),
        ('switch', dict(
            test_qml_file_path=switch_test_qml_file_path,
            objectName='test_switch'))
    ]

    def setUp(self):
        super().setUp()
        self.toggle = self.main_view.select_single(
            ubuntuuitoolkit.CheckBox, objectName=self.objectName)
        self.assertFalse(self.toggle.checked)

    def test_toggle_custom_proxy_object(self):
        self.assertIsInstance(self.toggle, ubuntuuitoolkit.CheckBox)

    def test_check_toggle(self):
        self.toggle.check()
        self.assertTrue(self.toggle.checked)

    def test_check_toggle_already_checked(self):
        self.toggle.check()
        with mock.patch.object(input.Pointer, 'click_object') as mock_click:
            self.toggle.check()
        self.assertFalse(mock_click.called)

    def test_uncheck_toggle(self):
        self.toggle.check()
        self.toggle.uncheck()
        self.assertFalse(self.toggle.checked)

    def test_uncheck_toggle_already_unchecked(self):
        with mock.patch.object(input.Pointer, 'click_object') as mock_click:
            self.toggle.uncheck()
        self.assertFalse(mock_click.called)

    def test_change_state_from_checked(self):
        self.toggle.check()
        self.toggle.change_state()
        self.assertFalse(self.toggle.checked)

    def test_change_state_from_unchecked(self):
        self.toggle.change_state()
        self.assertTrue(self.toggle.checked)

    def test_check_with_timeout(self):
        with mock.patch.object(
                ubuntuuitoolkit.CheckBox, 'change_state') as mock_change:
            self.toggle.check(timeout=1)

        mock_change.assert_called_once_with(1)

    def test_uncheck_with_timeout(self):
        self.toggle.check()
        with mock.patch.object(
                ubuntuuitoolkit.CheckBox, 'change_state') as mock_change:
            self.toggle.uncheck(timeout=1)

        mock_change.assert_called_once_with(1)

    def test_change_state_with_timeout(self):
        with mock.patch.object(self.toggle, 'pointing_device'):
            # mock the pointing device so the checkbox is not clicked.
            timestamp_before_call = time.time()
            self.assertRaises(AssertionError, self.toggle.change_state, 1)

        waiting_time = time.time() - timestamp_before_call
        self.assertThat(waiting_time, GreaterThan(1))
        self.assertThat(waiting_time, LessThan(2))
