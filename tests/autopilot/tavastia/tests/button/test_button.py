# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the Button Qml component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip
import os
from tavastia.tests import TavastiaTestCase

class EnabledButtonTests(TavastiaTestCase):
    """Tests for an enabled Button component."""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),"EnabledButtonTests")

    def test_can_select_button(self):
        """Must be able to select the Qml button component."""

        btn = self.app.select_single('Button')
        self.assertThat(btn, Not(Is(None)))


    def test_clicked_signal_emitted(self):
        """Clicking the button component must emit the clicked() signal."""
        
        btn = self.app.select_single('Button')
        signal = btn.watch_signal('clicked(QVariant)')

        self.mouse.move_to_object(btn)
        self.mouse.click()

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))


    def test_entered_signal_emitted(self):
        """The hoveredChanged() signal must be emitted when the mouse hovers over
        the button. Then the hovered property should be true"""

        btn = self.app.select_single('Button')
        signal = btn.watch_signal('hoveredChanged()')

        self.mouse.move_to_object(btn)

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))
        self.assertThat(btn.hovered, Eventually(Equals(True)))


    def test_exited_signal_emitted(self):
        """The hoveredChanged() signal must be emitted when the mouse is hovering
        over hovers the button, and is moved out. Then the hovered property should be 
        false"""

        btn = self.app.select_single('Button')

        # position mouse over button before starting to watch for signals
        self.mouse.move_to_object(btn)
        signal = btn.watch_signal('hoveredChanged()')

        # assuming the WM will never put the window over 0,0:
        self.mouse.move(0,0)

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))
        self.assertThat(btn.hovered, Eventually(Equals(False)))


    def test_can_press_button(self):
        """Test that when we click and hold the button down it's pressed property is set."""

        btn = self.app.select_single('Button')

        self.mouse.move_to_object(btn)
        self.mouse.press()
        self.addCleanup(self.mouse.release)

        self.assertThat(btn.pressed, Eventually(Equals(True)))


class DisabledButtonTests(TavastiaTestCase):
    """Tests for a disabled button component."""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),"DisabledButtonTests")

    def test_can_select_button(self):
        """Must be able to select the Qml button component."""

        btn = self.app.select_single('Button')
        self.assertThat(btn, Not(Is(None)))


    def test_button_is_disabled(self):
        """Test that the button really is disabled."""

        btn = self.app.select_single('Button')

        self.assertThat(btn.enabled, Equals(False))


    def test_clicked_signal_not_emitted(self):
        """Clicking a disabled button must not emit the clicked() signal."""

        btn = self.app.select_single('Button')
        signal = btn.watch_signal('clicked(QVariant)')

        self.mouse.move_to_object(btn)
        self.mouse.click()

        self.assertThat(signal.was_emitted, Equals(False))
        self.assertThat(signal.num_emissions, Equals(0))

class ButtonColorTests(TavastiaTestCase):
    """Tests for the button color properties."""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),"ButtonColorTests")

    def test_can_select_button(self):
        """Must be able to select the Qml button component."""

        btn = self.app.select_single('Button')
        self.assertThat(btn, Not(Is(None)))


    def test_button_has_correct_color(self):
        """Button component must have correct color."""

        btn = self.app.select_single('Button')
        self.assertThat(btn.color, Eventually(Equals([255,255,0,255])))


    def test_button_has_correct_pressed_color(self):
        """Button component must have correct pressed color."""

        btn = self.app.select_single('Button')
        self.assertThat(btn.pressedColor, Eventually(Equals([0,255,255,255])))


    def test_button_color_changes_on_mouse_press(self):
        """Button color must change to pressedColor when pressed with mouse."""

        btn = self.app.select_single('Button')

        self.mouse.move_to_object(btn)
        self.mouse.press()
        self.addCleanup(self.mouse.release)

        # this is hacky because the base rectangle in the button has no name. If
        # the component were named this would be a much more readable test...
        btnbase = self.app.select_single('QQuickRectangle', color=[0,255,255,255])

        self.assertThat(btnbase, Not(Is(None)))



class ButtonIconTests(TavastiaTestCase):
    """Tests for the button icon properties"""

    test_qml_file = "%s/%s.qml" % (os.path.dirname(os.path.realpath(__file__)),"ButtonIconTests")

    def test_can_select_button(self):
        """Must be able to select the Qml button component."""

        btn = self.app.select_single('Button')
        self.assertThat(btn, Not(Is(None)))


    def test_can_set_icon(self):
        """Must be able to set icon for the Button"""

        btn = self.app.select_single('Button')
        dir(btn)
        self.assertThat(btn.iconSource, Equals("call_icon.png"))


    def test_can_set_icon_position(self):
        """Must be able to set icon position for the Button"""

        btn = self.app.select_single('Button')
        self.assertThat(btn.iconPosition, Equals("right"))


