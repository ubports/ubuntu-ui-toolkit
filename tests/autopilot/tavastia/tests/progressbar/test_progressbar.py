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

from tavastia.tests import TavastiaTestCase

class ProgressBarTests(TavastiaTestCase):
    """Tests for ProgressBar component."""

    test_qml = dedent("""\
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Column {
        ProgressBar {
           id: progressBar
           minimumValue: 1
           maximumValue: 10
        }
        Button {
            text: "indeterminate"
            onClicked: {
                progressBar.indeterminate = !progressBar.indeterminate;
            }
        }
        Button {
            text: "minmax_values"
            onClicked: {
                progressBar.minimumValue = progressBar.minimumValue + 1;
                progressBar.maximumValue = progressBar.maximumValue + 10;
            }
        }
        Button {
            text: "reset values"
            onClicked: {
                progressBar.value = -10;
                progressBar.minimumValue = -10;
                progressBar.maximumValue = 10;
                timer.running = false;
            }
        }
        Button {
            text: "inc value"

            property bool running : timer.running

            onClicked: {
                timer.running = true;
            }
        }
        Timer {
            id: timer
            interval: 100
            running: false
            repeat: true
            onTriggered: {
                progressBar.value += 1;
                running = (progressBar.value!==progressBar.maximumValue) 
            }
        }
    }
    """)

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
        self.assertThat(obj.indeterminate, Equals(True))

    def test_can_set_minimum_and_maximum(self):
        """Must be able to set indeterminate"""

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
        self.assertThat(obj.minimumValue, Equals(old_minimumValue+1))
        self.assertThat(obj.maximumValue, Equals(old_maximumValue+10))

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
        
        # wait until it is done
        time.sleep(3)

        # the value should have changed but maximumValue and minimumValue should be the same
        self.assertThat(obj.value, Equals(maximumValue))
        self.assertThat(obj.maximumValue, Equals(maximumValue))
        self.assertThat(obj.minimumValue, Equals(minimumValue))

