# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the Ubuntu UI Toolkit Phone Component Showcase"""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from UbuntuUiToolkit.tests import UbuntuUiToolkitTestCase
from UbuntuUiToolkit.emulators.main_window import MainWindow

class GenericTests(UbuntuUiToolkitTestCase):
    """Generic tests for the Phone Component Showcase"""

    # Support both running from system and in the source directory
    runPath = os.path.dirname(os.path.realpath(__file__))
    localSourceFile = runPath + "/../../../../../demos/PhoneComponentShowcase.qml"
    if (os.path.isfile(localSourceFile)):
        print "Using local source directory"
        test_qml_file = localSourceFile
    else:
        print "Using system QML file"
        test_qml_file = "/usr/lib/ubuntu-ui-toolkit/demos/PhoneComponentShowcase.qml"

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.main_window.get_qml_view()
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible,Eventually(Equals(True)))

    def test_1_can_select_componentShowcase(self):
        """Must be able to select the Phone Component Showcase main qml file"""

        componentShowcase = self.main_window.get_object("QQuickRectangle", "phoneComponentShowcase")
        self.assertThat(componentShowcase, Not(Is(None)))
        self.assertThat(componentShowcase.visible, Eventually(Equals(True)))
