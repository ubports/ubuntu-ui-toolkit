# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2015 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit units helpers."""

import logging
import os
import testscenarios
import testtools

import fixtures

from ubuntuuitoolkit import (
    fixture_setup,
    units
)


logger = logging.getLogger(__name__)


class UnitsTestCase(testscenarios.TestWithScenarios,
                    testtools.TestCase):

    WIDTH_IN_GU = 20

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_units.UnitsTestCase.qml')

    scenarios = [
        ('with default GRID_UNIT_PX', {'grid_unit_px': '',
         'expected_pixels': 160}),
        ('with GRID_UNIT_PX environment variable set', {'grid_unit_px': '10',
         'expected_pixels': 200})
    ]

    def setUp(self):
        self.useFixture(fixtures.EnvironmentVariable(
            'GRID_UNIT_PX', self.grid_unit_px))
        self.useFixture(fixture_setup.InitctlEnvironmentVariable(
            global_=True, GRID_UNIT_PX=self.grid_unit_px))
        super().setUp()

    def test_gu(self):
        pixels = units.gu(self.WIDTH_IN_GU)
        self.assertEquals(pixels, self.expected_pixels)
