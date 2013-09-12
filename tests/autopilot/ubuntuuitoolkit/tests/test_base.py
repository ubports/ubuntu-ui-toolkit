# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
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

import mock
import testtools
from autopilot import input, platform

from ubuntuuitoolkit import base


class AppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Empty test case to be used by other tests."""
            
    def _runTest(self):
        pass

    def launch_application(self):
        pass


class TestUbuntuUIToolkitAppTestCase(testtools.TestCase):

    @unittest.skipIf(platform.model() != 'Desktop', 'Desktop only')
    def test_desktop_input_device_class(self):
        test = AppTestCase('_runTest')
        test.setUp()
        self.assertIs(test.input_device_class, input.Mouse)

    @unittest.skipIf(platform.model() == 'Desktop', 'Phablet only')
    def test_phablet_input_device_class(self):
        test = AppTestCase()
        test.setUp()
        self.assertIs(test.input_device_class, input.Touch)

    @mock.patch.object(
        base.UbuntuUIToolkitAppTestCase, 'is_desktop', return_value=True)
    def test_launch_application_with_source_on_desktop(self, mock_model):
        class AppFromSourceTestCase(base.UbuntuUIToolkitAppTestCase):
            application_launched_from_source = False
            def application_source_exists(self):
                return True
            def launch_application_from_source(self):
                self.application_launched_from_source = True
            def runTest(self):
                pass
        test = AppFromSourceTestCase()
        test.setUp()
        self.assertTrue(test.application_launched_from_source)

    @mock.patch.object(
        base.UbuntuUIToolkitAppTestCase, 'is_desktop', return_value=False)
    def test_launch_application_with_source_on_phablet(self, mock_model):
        class AppFromSourceTestCase(base.UbuntuUIToolkitAppTestCase):
            installed_application_launched = False
            def application_source_exists(self):
                return True
            def launch_installed_application(self):
                self.installed_application_launched = True
            def runTest(self):
                pass
        test = AppFromSourceTestCase()
        test.setUp()
        self.assertTrue(test.installed_application_launched)

    def test_launch_installed_application(self):
        class InstalledAppTestCase(base.UbuntuUIToolkitAppTestCase):
            installed_application_launched = False
            def application_source_exists(self):
                return False
            def launch_installed_application(self):
                self.installed_application_launched = True
            def runTest(self):
                pass
        test = InstalledAppTestCase()
        test.setUp()
        self.assertTrue(test.installed_application_launched)
