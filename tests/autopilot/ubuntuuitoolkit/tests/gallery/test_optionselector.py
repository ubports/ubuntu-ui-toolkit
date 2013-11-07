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

"""Tests for the Ubuntu UI Toolkit Gallery - OptionSelector component"""
import time
from autopilot.matchers import Eventually
from testtools.matchers import Equals
from ubuntuuitoolkit.tests import FlickDirection
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class OptionSelectorTests(GalleryTestCase):
    """Generic tests for the Gallery"""

    def test_optionselector_collapsed(self):
        item = "Option Selector"
        self.loadItem(item)
        self.checkPageHeader(item)
        collapsed = self.getObject("optionselector_collapsed")
        styleditem = collapsed.select_single('StyledItem',
                                             objectName='listContainer')

        self.assertThat(collapsed.selectedIndex, Equals(0))
        self.pointing_device.click_object(collapsed)
        self.assertThat(styleditem.currentlyExpanded, Eventually(Equals(True)))
        #try to search the following list entry few times
        #as it may not be available immediately.
        for t in range(0, 9):
            try:
                selectedValue = collapsed.select_single('Label',
                                                        text='Value 4')
                self.assertIsNotNone(selectedValue)
            except:
                time.sleep(1)

        self.pointing_device.click_object(selectedValue)
        self.assertThat(collapsed.selectedIndex, Eventually(Equals(3)))
        self.assertThat(styleditem.currentlyExpanded, Eventually(Equals(False)))

    def test_optionselector_expanded(self):
        item = "Option Selector"
        self.loadItem(item)
        self.checkPageHeader(item)
        expanded = self.getObject("optionselector_expanded")

        self.assertThat(expanded.selectedIndex, Equals(0))
        selectedValue = expanded.select_single('Label', text='Value 4')
        self.assertIsNotNone(selectedValue)
        self.pointing_device.click_object(selectedValue)
        self.assertThat(expanded.selectedIndex, Eventually(Equals(3)))

        selectedValue = expanded.select_single('Label', text='Value 1')
        self.assertIsNotNone(selectedValue)
        self.pointing_device.click_object(selectedValue)
        self.assertThat(expanded.selectedIndex, Eventually(Equals(0)))

    def test_optionselector_custommodel(self):
        item = "Option Selector"
        self.loadItem(item)
        self.checkPageHeader(item)

        custommodel = self.getObject("optionselector_custommodel")
        flickable = self.main_view.select_single('QQuickFlickable')
        self.assertIsNotNone(flickable)

        #Flick upward to reveal the hidden ui element.
        self.reveal_item_by_flick(custommodel, flickable, FlickDirection.UP)
        self.assertThat(flickable.flicking, Eventually(Equals(False)))

        self.assertThat(custommodel.selectedIndex, Equals(0))
        selectedValue = custommodel.select_single('Label', text='Name 4')
        self.assertIsNotNone(selectedValue)
        self.pointing_device.click_object(selectedValue)
        self.assertThat(custommodel.selectedIndex, Eventually(Equals(3)))

        selectedValue = custommodel.select_single('Label', text='Name 1')
        self.assertIsNotNone(selectedValue)
        self.pointing_device.click_object(selectedValue)
        self.assertThat(custommodel.selectedIndex, Eventually(Equals(0)))

        #scroll the page downward now.
        collapsed = self.getObject("optionselector_collapsed")
        self.reveal_item_by_flick(collapsed, flickable, FlickDirection.DOWN)
        self.assertThat(flickable.flicking, Eventually(Equals(False)))
