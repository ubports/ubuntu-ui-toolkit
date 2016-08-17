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

import unittest

import autopilot
from autopilot import platform, input
from unittest import mock

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class CheckAutopilotVersionTestCase(unittest.TestCase):

    def test_lower_version_should_raise_exception(self):
        with mock.patch.object(autopilot, 'version', '1.3'):
            self.assertRaises(
                ubuntuuitoolkit.ToolkitException,
                ubuntuuitoolkit.check_autopilot_version)

    def test_required_version_should_succeed(self):
        with mock.patch.object(autopilot, 'version', '1.4'):
            ubuntuuitoolkit.check_autopilot_version()

    def test_higher_version_should_succeed(self):
        with mock.patch.object(autopilot, 'version', '1.5'):
            ubuntuuitoolkit.check_autopilot_version()


# FIXME: this class is not used anywhere, and QMLStringAppTestCase subclasses
#   no longer work (see bug 1578319), so the code below can be deleted.
class UbuntuUIToolkitCustomProxyObjectBaseTestCase(tests.QMLStringAppTestCase):

    def test_pointing_device(self):
        self.assertIsInstance(self.app.pointing_device, input.Pointer)

    @unittest.skipIf(platform.model() != 'Desktop', 'Desktop only')
    def test_pointing_device_in_desktop(self):
        self.assertIsInstance(self.app.pointing_device._device, input.Mouse)

    @unittest.skipIf(platform.model() == 'Desktop', 'Phablet only')
    def test_pointing_device_in_phablet(self):
        self.assertIsInstance(self.app.pointing_device._device, input.Touch)

    def test_custom_proxy_objects_should_check_version_on_init(self):
        check_name = (
            'ubuntuuitoolkit._custom_proxy_objects._common.'
            'check_autopilot_version')
        with mock.patch(check_name, autospec=True) as mock_check:
            # Instantiate any custom proxy object.
            self.main_view

        mock_check.assert_called_once_with()
