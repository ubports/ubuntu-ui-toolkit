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

from autopilot.matchers import Eventually
from testtools.matchers import Is, Not, Equals

from ubuntuuitoolkit import emulators
from ubuntuuitoolkit.tests import gallery


class GenericTests(gallery.GalleryTestCase):
    """Generic tests for the Gallery"""

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.main_view
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible, Eventually(Equals(True)))

    def test_navigation(self):
        item = "Navigation"
        self.loadItem(item)
        self.checkPageHeader(item)

    def test_slider(self):
        item = "Slider"
        self.loadItem(item)
        self.checkPageHeader(item)

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
        item = "Progress and activity"
        self.loadItem(item)
        self.checkPageHeader(item)

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

        item = "Ubuntu Shape"
        self.loadItem(item)
        self.checkPageHeader(item)

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
        'labels', 'listItems', 'dialogs', 'popovers', 'sheets', 'animations'
    ]

    scenarios = [
        (name, dict(
            element_name=name+'Element',
            template_name=name+'Template'))
        for name in names
    ]

    def test_open_page(self):
        list_view = self.main_view.select_single(
            emulators.QQuickListView, objectName="widgetList")
        list_view.click_element(self.element_name)
        element = self.main_view.select_single(
            'Standard', objectName=self.element_name)
        element.selected.wait_for(True)
        self.checkPageHeader(element.text)
        if self.template_name == 'textinputsTemplate':
            page_type = 'TextInputs'
        else:
            page_type = 'Template'
        self.main_view.wait_select_single(
            page_type, objectName=self.template_name)
        # TODO check that the template is visible. --elopio - 2013-11-28
