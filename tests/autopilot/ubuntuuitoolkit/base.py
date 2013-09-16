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

from autopilot import (
    input,
    platform,
    testcase
)


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
