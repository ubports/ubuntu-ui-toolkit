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

from autopilot import input
from testtools.matchers import GreaterThan, LessThan
from unittest import mock

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


TEST_QML_WITH_CHECKBOX = ("""
import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Item {
        CheckBox {
            checked: false
            objectName: "test_checkbox"
        }
    }
}
""")


TEST_QML_WITH_SWITCH = ("""
import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Item {
        Switch {
            checked: false
            objectName: "test_switch"
        }
    }
}
""")


class ToggleTestCase(tests.QMLStringAppTestCase):

    scenarios = [
        ('checkbox', dict(
            test_qml=TEST_QML_WITH_CHECKBOX, objectName='test_checkbox')),
        ('switch', dict(
            test_qml=TEST_QML_WITH_SWITCH, objectName='test_switch'))
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
