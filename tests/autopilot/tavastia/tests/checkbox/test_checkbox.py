# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the CheckBox Qml component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from tavastia.tests import TavastiaTestCase

class EnabledCheckBoxTests(TavastiaTestCase):
    """Tests for an enabled CheckBox component."""

    test_qml_file = "%s/EnabledCheckBoxTests.qml" % os.path.dirname(os.path.realpath(__file__))

    def test_can_select_checkbox(self):
        """Must be able to select the Qml CheckBox component."""

        obj = self.app.select_single('CheckBox')
        self.assertThat(obj, Not(Is(None)))

    def test_clicked_signal_emitted(self):
        """Clicking the checkbox component must emit the clicked() signal."""

        obj = self.app.select_single('CheckBox')
        signal = obj.watch_signal('clicked(QVariant)')

        self.assertThat(obj.checked, Eventually(Equals(False)))

        self.mouse.move_to_object(obj)
        self.mouse.click()

        self.assertThat(obj.checked, Eventually(Equals(True)))

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))

class DisabledCheckBoxTests(TavastiaTestCase):
    """Tests for an enabled CheckBox component."""

    test_qml_file = "%s/DisabledCheckBoxTests.qml" % os.path.dirname(os.path.realpath(__file__))

    def test_clicked_signal_not_emitted(self):
        """Clicking a disabled CheckBox must not emit the clicked() signal."""

        obj = self.app.select_single('CheckBox')
        signal = obj.watch_signal('clicked(QVariant)')

        self.assertThat(obj.checked, Equals(False))

        self.mouse.move_to_object(obj)
        self.mouse.click()

        self.assertThat(obj.checked, Eventually(Equals(False)))

        self.assertThat(signal.was_emitted, Equals(False))
        self.assertThat(signal.num_emissions, Equals(0))

