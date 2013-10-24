# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
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
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class GenericTests(GalleryTestCase):
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

    def test_toggles(self):
        item = "Toggles"
        self.checkListItem(item)
        self.loadItem(item)
        self.checkPageHeader(item)

        # check default states
        item_data = [
            ["checkbox_unchecked", False, True],
            ["checkbox_checked", True, True],
            ["checkbox_disabled_unchecked", False, False],
            ["checkbox_disabled_checked", True, False],
            ["switch_unchecked", False, True],
            ["switch_checked", True, True],
            ["switch_disabled_unchecked", False, False],
            ["switch_disabled_checked", True, False]
        ]

        for data in item_data:
            objName = data[0]
            objChecked = data[1]
            objEnabled = data[2]

            obj = self.getObject(objName)
            self.assertThat(obj.checked, Equals(objChecked))
            self.assertThat(obj.enabled, Equals(objEnabled))

            # try to interact with objects
            self.tap(objName)

            if (obj.enabled):
                self.assertThat(obj.checked, Not(Equals(objChecked)))
            else:
                self.assertThat(obj.checked, Equals(objChecked))

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

    def test_all_pages_load(self):
        contentLoader, listView = self.getWidgetLoaderAndListView()
        for child in listView.select_many('Standard'):
            item = child.text
            # FIXME: Text Field doesn't work
            if item == 'Text Field':
                continue
            self.loadItem(item)
            self.checkPageHeader(item)
            assert len(self.app.select_many('Template')) > 0, \
                ('%s can be loaded in gallery' % item)
