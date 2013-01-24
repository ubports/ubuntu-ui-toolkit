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

    def checkListItem(self, itemText):
        item = self.app.select_single('Standard',text=itemText)
        self.assertThat(item, Not(Is(None)));

    def getListItem(self, itemText):
        return self.app.select_single('Standard',text=itemText)

    def getWidgetLoaderAndListView(self):
        widgetLoader = self.app.select_single(objectName="widgetLoader")
        listView = self.app.select_single(objectName="widgetList")
        self.assertThat(listView, Not(Is(None)));
        self.assertThat(listView.visible, Eventually(Equals(True)));
        return (widgetLoader, listView)

    def loadItem(self, item):
        widgetLoader = self.app.select_single(objectName="widgetLoader")
        self.selectItem(item)
        self.assertThat(widgetLoader.progress,Eventually(Equals(1.0)))
        loadedPage = self.app.select_single(objectName=item)
        self.assertThat(loadedPage, Not(Is(None)));
        self.assertThat(loadedPage.visible, Eventually(Equals(True)));  

    def drag(self, itemText, itemTextTo):
        item = self.getListItem(itemText)
        itemTo = self.getListItem(itemTextTo)

        self.mouse.move_to_object(item)
        self.mouse.press()
        self.mouse.move_to_object(itemTo)
        self.mouse.release()

    def selectItem(self, itemText):
        item = self.getListItem(itemText)
        self.assertThat(item.selected, Eventually(Equals(False)))

        self.mouse.move_to_object(item)
        self.mouse.click()

        self.assertThat(item.selected, Eventually(Equals(True)))

    ###########################################################
    # Tests

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

