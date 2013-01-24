# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the Ubuntu UI Toolkit Component Showcase"""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from UbuntuUiToolkit.tests import UbuntuUiToolkitTestCase


class GenericTests(UbuntuUiToolkitTestCase):
    """Generic tests for the Component Showcase"""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),"../../../../../demos/ComponentShowcase")

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.app.select_single('QQuickRootItem')
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible,Eventually(Equals(True)))

    def test_1_can_select_componentShowcase(self):
        """Must be able to select the Component Showcase main qml file"""

        componentShowcase = self.app.select_single(objectName="componentShowcase")
        self.assertThat(componentShowcase, Not(Is(None)))
        self.assertThat(componentShowcase.visible, Eventually(Equals(True)))

    def test_2_can_select_listview(self):
        """Must be able to select the listview from main"""    

        widgetLoader,listView = self.getWidgetLoaderAndListView();

        items = [
                        "Resolution Independence",
                        "Theming",
                        "Ubuntu Shape", 
                        "Buttons", 
                        "Tabs (classic)", 
                        "Tabs (flickable)", 
                        "List Items", 
                        "Page Stack",
                        "Switch", 
                        "Check Box", 
                        "Activity Indicator",
                        "Progress Bar", 
                    ]                  


        for item in items:
            self.checkListItem(item)
            self.loadItem(item)


        # scroll view to expose more items
        self.drag("Progress Bar","Theming")

        # now that we have more items, lets continue
        items = [
                        "Slider", 
                        "Text Input", 
                        "Text Area",
                        "Scrollbar", 
                        "Popups",
                        "GIcon Provider", 
                ]

        for item in items:
            self.checkListItem(item)
            self.loadItem(item)            

