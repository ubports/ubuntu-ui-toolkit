# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the QML Slider component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from tavastia.tests import TavastiaTestCase

class SliderTests(TavastiaTestCase):
    """Tests for Slider component."""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),self.__class__.__name__)

    def test_can_select_slider(self):
        """Must be able to select the Qml Slider component."""

        obj = self.app.select_single('Slider')
        self.assertThat(obj, Not(Is(None)))
    
    def test_can_set_live(self):
        """Must be able to set live"""

        obj = self.app.select_single('Slider')
        btn = self.app.select_single('Button', text="live")

        # default value is False
        self.assertThat(obj.live, Equals(False))

        self.mouse.move_to_object(btn)
        self.mouse.click()

        # we should be able to set the value within the QML, check Button::onClicked
        self.assertThat(obj.live, Eventually(Equals(True)))

    def test_can_set_minimum_and_maximum(self):
        """Must be able to set minimum and maximum values"""

        obj = self.app.select_single('Slider')
        btn = self.app.select_single('Button', text="minmax_values")

        self.assertThat(obj.minimumValue, Equals(0))
        self.assertThat(obj.maximumValue, Equals(11))

        old_minimumValue = obj.minimumValue
        old_maximumValue = obj.maximumValue

        self.mouse.move_to_object(btn)
        self.mouse.click()

        # we should be able to set the value within the QML, check Button::onClicked
        self.assertThat(obj.minimumValue, Eventually(Equals(old_minimumValue+1)))
        self.assertThat(obj.maximumValue, Eventually(Equals(old_maximumValue+10)))

    def test_can_set_value(self):
        """Must be able to set value"""

        obj = self.app.select_single('Slider')
        btnreset = self.app.select_single('Button', text="reset values")
        btnvalue = self.app.select_single('Button', text="inc value")

        # lets reset values
        self.mouse.move_to_object(btnreset)
        self.mouse.click()
        
        # read current values
        self.assertThat(obj.maximumValue, Eventually(Equals(10)))
        self.assertThat(obj.minimumValue, Eventually(Equals(-10)))

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

