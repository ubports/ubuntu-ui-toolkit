# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery"""

import testscenarios

import ubuntuuitoolkit
from ubuntuuitoolkit import ubuntu_scenarios
from ubuntuuitoolkit.tests import gallery


class GalleryAppTestCase(gallery.GalleryTestCase):
    """Generic tests for the Gallery"""

    scenarios = ubuntu_scenarios.get_device_simulation_scenarios()

    def test_select_main_view_must_return_main_window_emulator(self):
        main_view = self.main_view
        self.assertIsInstance(main_view, ubuntuuitoolkit.MainView)

    def test_slider(self):
        self.open_page('slidersElement')

        item_data = [
            ["slider_standard"],
            ["slider_live"],
            ["slider_range"]
        ]

        for data in item_data:
            objName = data[0]
            self.getObject(objName)
            self.tap(objName)

            # TODO: move slider value

    def test_progress_and_activity(self):
        self.open_page('progressBarsElement')

        item_data = [
            ["progressbar_standard"],
            ["progressbar_indeterminate"],
            ["activityindicator_standard"]
        ]

        for data in item_data:
            objName = data[0]
            self.getObject(objName)
            self.tap(objName)

            # TODO: check for properties

    def test_ubuntushape(self):
        # Flaky test case
        # FIXME: https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1308979
        return

        self.open_page('ubuntuShapesElement')

        item_data = [
            ["ubuntushape_color_hex"],
            ["ubuntushape_color_lightblue"],
            ["ubuntushape_color_darkgray"],
            ["ubuntushape_image"],
            ["ubuntushape_radius_small"],
            ["ubuntushape_radius_medium"],
            ["ubuntushape_sizes_15_6"],
            ["ubuntushape_sizes_10_14"]
        ]

        for data in item_data:
            objName = data[0]
            self.getObject(objName)


class OpenPagesTestCase(gallery.GalleryTestCase):

    names = [
        'navigation', 'toggles', 'buttons', 'sliders', 'textinputs',
        'optionSelectors', 'pickers', 'progressBars', 'ubuntuShapes', 'icons',
        'labels', 'listItems', 'ubuntuListView', 'dialogs', 'popovers',
        'sheets', 'animations'
    ]

    pages_scenarios = [
        (name, dict(
            element_name=name+'Element',
            template_name=name+'Template'))
        for name in names
    ]

    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        pages_scenarios)

    def test_open_page(self):
        self.open_page(self.element_name)
        self.main_view.wait_select_single(objectName=self.template_name)
