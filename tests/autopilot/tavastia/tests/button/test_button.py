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

from tavastia.tests import TavastiaTestCase

class EnabledButtonTests(TavastiaTestCase):

    """Tests for an enabled Button component."""

    test_qml = dedent("""\
    import QtQuick 1.1
    import Ubuntu.Components 0.1

    Button {
       id: button
       text: "Hello World"
    }
    """)


    def test_can_select_button(self):
        """Must be able to select the Qml button component."""
        btn = self.app.select_single('Button')
        self.assertThat(btn, Not(Is(None)))

    def test_clicked_signal_emitted(self):
        """Clicking the button component must emit the clicked() signal."""
        btn = self.app.select_single('Button')
        signal = btn.watch_signal('clicked()')

        self.mouse.move_to_object(btn)
        self.mouse.click()

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))

    def test_entered_signal_emitted(self):
        """The entered() signal must be emitted when the mouse hovers the button.

        Note: this test fails. It seems the entered() signal is never triggered.
        A real bug?

        """
        btn = self.app.select_single('Button')
        mouse_area = btn.get_children_by_type('QDeclarativeMouseArea')[0]
        signal = mouse_area.watch_signal('entered()')

        self.mouse.move_to_object(btn)

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))

    def test_exited_signal_emitted(self):
        """The exited() signal must be emitted when the mouse hovers the button.

        Note: This test fails - it seems the exited() signal is never fired. This
        seems like a but to me...

        """
        btn = self.app.select_single('Button')
        self.mouse.move_to_object(btn)
        mouse_area = btn.get_children_by_type('QDeclarativeMouseArea')[0]
        signal = mouse_area.watch_signal('exited()')

        # assuming the WM will never put the window over 0,0:
        self.mouse.move(0,0)

        self.assertThat(signal.was_emitted, Equals(True))
        self.assertThat(signal.num_emissions, Equals(1))

    def test_can_press_button(self):
        """Test that when we click and hold the button down it's pressed property is set."""
        btn = self.app.select_single('Button')

        self.mouse.move_to_object(btn)
        self.mouse.press()
        self.addCleanup(self.mouse.release)

        self.assertThat(btn.pressed, Eventually(Equals(True)))


class DisabledButtonTests(TavastiaTestCase):

    """Tests for a disabled button component."""

    test_qml = dedent("""\
    import QtQuick 1.1
    import Ubuntu.Components 0.1

    Button {
       id: button
       text: "Disabled button"
       enabled: False
    }
    """)

    def test_button_is_disabled(self):
        """Test that the button really is disabled.

        Note: this test fails. This seems like a legitimate bug to me...
        """
        btn = self.app.select_single('Button')

        self.assertThat(btn.enabled, Equals(False))

    def test_clicked_signal_not_emitted(self):
        """Clicking a disabled button must not emit the clicked() signal.

        This test fails. I'm not sure if it's a symptom of the enabled property
        not working, or perhaps this is desired behavior?

        """
        btn = self.app.select_single('Button')
        signal = btn.watch_signal('clicked()')

        self.mouse.move_to_object(btn)
        self.mouse.click()

        self.assertThat(signal.was_emitted, Equals(False))
        self.assertThat(signal.num_emissions, Equals(0))


class ButtonColorTests(TavastiaTestCase):

    """Tests for the button color properties."""

    test_qml = dedent("""\
    import QtQuick 1.1
    import Ubuntu.Components 0.1

    Button {
       id: button
       text: "Coloured Button"
       color: "#FFFF00"
       pressedColor: "#00FFFF"
    }
    """)


    def test_button_has_correct_color(self):
        """Button component must have correct color."""
        btn = self.app.select_single('Button')
        self.assertThat(btn.color, Equals([255,255,0,255]))

    def test_button_has_correct_pressed_color(self):
        """Button component must have correct pressed color."""
        btn = self.app.select_single('Button')
        self.assertThat(btn.pressedColor, Equals([0,255,255,255]))

    def test_button_color_changes_on_mouse_press(self):
        """Button color must change to pressedColor when pressed with mouse."""
        btn = self.app.select_single('Button')

        self.mouse.move_to_object(btn)
        self.mouse.press()
        self.addCleanup(self.mouse.release)

        # this is hacky because the base rectangle in the button has no name. If
        # the component were named this would be a much more readable test...
        btnbase = self.app.select_single('QDeclarativeRectangle', color=[0,255,255,255])

        self.assertThat(btnbase, Not(Is(None)))

# This is a little hack to allow you to launch the test_qml in the viewer so it
# shows up in the autopilot vis tool. Not really needed at all...
if __name__ == '__main__':
    ButtonColorTests('test_button_has_correct_color').launch_test_qml()
