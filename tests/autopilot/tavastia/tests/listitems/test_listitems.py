# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the QML ListItems component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from tavastia.tests import TavastiaTestCase

class ListItemsTests(TavastiaTestCase):
    """Tests for ListItems component."""

    test_qml_file = "%s/ListItemsTests.qml" % os.path.dirname(os.path.realpath(__file__))

    def test_can_select_header(self):
        """Must be able to select the Qml ListItem.Header component."""

        obj = self.app.select_single('Header')
        self.assertThat(obj, Not(Is(None)))

    def test_can_select_standard(self):
        """Must be able to select the Qml ListItem.Standard component."""

        obj = self.app.select_single('Standard',text="Standard")
        self.assertThat(obj, Not(Is(None)))
    
    def test_can_select_singlevalue(self):
        """Must be able to select the Qml ListItem.SingleValue component."""

        obj = self.app.select_single('SingleValue')
        self.assertThat(obj, Not(Is(None)))

    def test_can_select_multivalue(self):
        """Must be able to select the Qml ListItem.MultiValue component."""

        obj = self.app.select_single('MultiValue')
        self.assertThat(obj, Not(Is(None)))

    def test_can_select_subtitled(self):
        """Must be able to select the Qml ListItem.Subtitled component."""

        obj = self.app.select_single('Subtitled')
        self.assertThat(obj, Not(Is(None)))

    def test_can_select_caption(self):
        """Must be able to select the Qml ListItem.Caption component."""

        obj = self.app.select_single('Caption')
        self.assertThat(obj, Not(Is(None)))

    def test_can_select_ValueSelector(self):
        """Must be able to select the Qml ListItem.ValueSelector component."""

        obj = self.app.select_single('ValueSelector')
        self.assertThat(obj, Not(Is(None)))
