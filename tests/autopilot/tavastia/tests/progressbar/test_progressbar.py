# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the ProgressBar Qml component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import time
import os
from tavastia.tests import TavastiaTestCase

class ProgressBarTests(TavastiaTestCase):
    """Tests for ProgressBar component."""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),self.__class__.__name__)

    def test_can_select_progressbar(self):
        """Must be able to select the Qml ProgressBar component."""

        obj = self.app.select_single('ProgressBar')
        self.assertThat(obj, Not(Is(None)))
    
    def test_can_set_indeterminate(self):
        """Must be able to set indeterminate"""

        obj = self.app.select_single('ProgressBar')
        btn = self.app.select_single('Button', text="indeterminate")

        # default value is False
        self.assertThat(obj.indeterminate, Equals(False))

        self.mouse.move_to_object(btn)
        self.mouse.click()

        # we should be able to set the value within the QML, check Button::onClicked
        self.assertThat(obj.indeterminate, Eventually(Equals(True)))

    def test_can_set_minimum_and_maximum(self):
        """Must be able to set minimum and maximum values"""

        obj = self.app.select_single('ProgressBar')
        btn = self.app.select_single('Button', text="minmax_values")

        # default value is False
        self.assertThat(obj.minimumValue, Equals(1))
        self.assertThat(obj.maximumValue, Equals(10))

        old_minimumValue = obj.minimumValue
        old_maximumValue = obj.maximumValue

        self.mouse.move_to_object(btn)
        self.mouse.click()

        # we should be able to set the value within the QML, check Button::onClicked
        self.assertThat(obj.minimumValue, Eventually(Equals(old_minimumValue+1)))
        self.assertThat(obj.maximumValue, Eventually(Equals(old_maximumValue+10)))

    def test_can_set_value(self):
        """Must be able to set value"""

        obj = self.app.select_single('ProgressBar')
        btnreset = self.app.select_single('Button', text="reset values")
        btnvalue = self.app.select_single('Button', text="inc value")

        # lets reset values
        self.mouse.move_to_object(btnreset)
        self.mouse.click()
        
        # read current values
        self.assertThat(obj.maximumValue, Equals(10))
        self.assertThat(obj.minimumValue, Equals(-10))

        value = obj.value
        maximumValue = obj.maximumValue
        minimumValue = obj.minimumValue

        # start the qml timer which should increase the value
        self.mouse.move_to_object(btnvalue)
        self.mouse.click()

        # the value should have changed but maximumValue and minimumValue should be the same
        self.assertThat(obj.value, Eventually(Equals(maximumValue)))
        self.assertThat(obj.maximumValue, Eventually(Equals(maximumValue)))
        self.assertThat(obj.minimumValue, Eventually(Equals(minimumValue)))

