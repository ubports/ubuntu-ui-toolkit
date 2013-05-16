# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the Ubuntu UI Toolkit Gallery"""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
import time
from UbuntuUiToolkit.tests import UbuntuUiToolkitTestCase
from UbuntuUiToolkit.emulators.main_window import MainWindow


class GenericTests(UbuntuUiToolkitTestCase):
    """Generic tests for the Gallery"""

    # Support both running from system and in the source directory
    runPath = os.path.dirname(os.path.realpath(__file__))
    localSourceFile = runPath + "/../../../../../examples/ubuntu-ui-toolkit-gallery/ubuntu-ui-toolkit-gallery.qml"
    if (os.path.isfile(localSourceFile)):
        print "Using local source directory"
        test_qml_file = localSourceFile
    else:
        print "Using system QML file"
        test_qml_file = "/usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery/ubuntu-ui-toolkit-gallery.qml"

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.main_window.get_qml_view()
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible,Eventually(Equals(True)))

    def test_1_can_select_listview(self):
        """Must be able to select the listview from main"""    

        contentLoader,listView = self.getWidgetLoaderAndListView();

        # Don't have the first, already selected item as the first item to check
        items = [
                        "Toggles", 
                        "Buttons", 
                        "Slider", 
                        "Text Field", 
                        "Progress and activity", 
                        "Ubuntu Shape", 
                        "Icons", 
                        "Label",
                        "List Items", 
                    ]                  


        for item in items:
            self.checkListItem(item)
            self.loadItem(item)


        # scroll view to expose more items
        self.drag("Icons","Text Field")

        # Wait for the scrolling to finish, the next click fails on the 
        # slower Intel machine but succeeds on AMD and NVIDIA.
        # (LP: #1180226)
        time.sleep(0)

        # now that we have more items, lets continue
        items = [
                        "Dialog", 
                        "Popover",
                ]

        for item in items:
            self.checkListItem(item)
            self.loadItem(item)            

