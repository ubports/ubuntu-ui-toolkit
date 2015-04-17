# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014, 2015 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery - OptionSelector component."""

import ubuntuuitoolkit
from ubuntuuitoolkit import ubuntu_scenarios
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class OptionSelectorTestCase(GalleryTestCase):

    scenarios = ubuntu_scenarios.get_device_simulation_scenarios()

    def setUp(self):
        super().setUp()
        self.open_page('optionSelectorsElement')

    def test_select_option_from_collapsed_optionselector(self):
        collapsed_option_selector = self.main_view.select_single(
            ubuntuuitoolkit.OptionSelector,
            objectName='optionselector_collapsed')

        self.assertEqual(
            collapsed_option_selector.get_selected_text(), 'Value 1')

        collapsed_option_selector.select_option('Label', text='Value 4')
        self.assertEqual(collapsed_option_selector.get_selected_index(), 3)

    def test_select_option_from_expanded_optionselector(self):
        expanded_option_selector = self.main_view.select_single(
            ubuntuuitoolkit.OptionSelector,
            objectName='optionselector_expanded')

        self.assertEqual(
            expanded_option_selector.get_selected_text(), 'Value 1')

        expanded_option_selector.select_option('Label', text='Value 4')
        self.assertEqual(expanded_option_selector.get_selected_index(), 3)

    def test_select_option_from_optionselector_with_custom_model(self):
        option_selector_with_custom_model = self.main_view.select_single(
            ubuntuuitoolkit.OptionSelector,
            objectName='optionselector_custommodel')
        option_selector_with_custom_model.swipe_into_view()

        self.assertEqual(
            option_selector_with_custom_model.get_selected_index(), 0)

        option_selector_with_custom_model.select_option(
            'Label', text='Name 4')
        self.assertEqual(
            option_selector_with_custom_model.get_selected_index(), 3)
