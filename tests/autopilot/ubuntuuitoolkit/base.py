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

"""Base classes for Autopilot tests using the Ubuntu UI Toolkit."""

import os
import subprocess

from autopilot import (
    input,
    platform,
    testcase
)


def get_host_multiarch():
    if 'deb_host_multiarch' not in os.environ:
        # Discard errors: the one known message is "GCC not installed"
        os.environ['deb_host_multiarch'] = subprocess.check_output(
            ["dpkg-architecture", "-qDEB_HOST_MULTIARCH"],
            universal_newlines=True, stderr=subprocess.PIPE).strip()
    return os.environ['deb_host_multiarch']

def get_qmlscene_launch_command():
    """Return the command to launch qmlscene for autopilot tests."""
    # We need to specify qt5 because qtchooser doesn't have a default
    # configuration on devices and it seems the environment variable
    # QT_SELECT=qt5 doesn't work for autopilot tests. --Mirv - 2013-10-03
    arch = get_host_multiarch()
    return '/usr/lib/{}/qt5/bin/qmlscene'.format(arch)


class UbuntuUIToolkitAppTestCase(testcase.AutopilotTestCase):
    """Autopilot test case for applications using the Ubuntu UI Toolkit."""

    def setUp(self):
        super(UbuntuUIToolkitAppTestCase, self).setUp()
        self.input_device_class = self._get_input_device_class()

    def _get_input_device_class(self):
        if platform.model() == 'Desktop':
            return input.Mouse
        else:
            return input.Touch
