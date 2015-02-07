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

import os

import fixtures

from ubuntuuitoolkit import (
    fixture_setup,
    tests,
    units
)


class UnitsTestCase(tests.QMLFileAppTestCase):

    BUTTON_WIDTH_IN_GU = 20

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_units.UnitsTestCase.qml')

    scenarios = [
        ('with default GRID_UNIT_PX', {'grid_unit_px': ''}),
        ('with GRID_UNIT_PX environment variable set', {'grid_unit_px': '10'})
    ]

    def setUp(self):
        self.useFixture(fixtures.EnvironmentVariable(
            'GRID_UNIT_PX', self.grid_unit_px))
        self.useFixture(fixture_setup.InitctlEnvironmentVariable(
            GRID_UNIT_PX=self.grid_unit_px))
        super(UnitsTestCase, self).setUp()
        self.button = self.main_view.select_single(objectName='button')
        self.label = self.main_view.select_single(objectName='clicked_label')

    def test_click_inside_button_using_grid_units(self):
        x, y, _, height = self.button.globalRect
        self.pointing_device.move(
            x + units.gu(self.BUTTON_WIDTH_IN_GU - 1), y + height // 2)
        self.pointing_device.click()

        self.assertEqual('Clicked.', self.label.text)

    def test_click_outside_button_using_grid_units(self):
        x, y, _, height = self.button.globalRect
        self.pointing_device.move(
            x + units.gu(self.BUTTON_WIDTH_IN_GU + 1), y + height // 2)
        self.pointing_device.click()

        self.assertEqual('Not clicked.', self.label.text)
