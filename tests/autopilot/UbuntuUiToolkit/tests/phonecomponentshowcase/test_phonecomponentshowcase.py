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

class GenericTests(UbuntuUiToolkitTestCase):
    """Generic tests for the Phone Component Showcase"""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),"../../../../../demos/PhoneComponentShowcase")

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.app.select_single('QQuickRootItem')
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible,Eventually(Equals(True)))

    def test_1_can_select_componentShowcase(self):
        """Must be able to select the Phone Component Showcase main qml file"""

        componentShowcase = self.app.select_single(objectName="phoneComponentShowcase")
        self.assertThat(componentShowcase, Not(Is(None)))
        self.assertThat(componentShowcase.visible, Eventually(Equals(True)))
