# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Tests for the Ubuntu UI Toolkit Gallery - Button component"""

import testscenarios
from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntuuitoolkit import ubuntu_scenarios
from ubuntuuitoolkit.tests import gallery

import os


class ButtonsTestCase(gallery.GalleryTestCase):

    buttons_scenarios = [
        ('standard button', dict(
            button_name="button_text", is_enabled=True, color=None, icon=None,
            text="Call")),
        ('button with color', dict(
            button_name="button_color", is_enabled=True,
            color=[0, 161, 50, 255], icon=None, text="Call")),
        ('button with icon', dict(
            button_name="button_iconsource", is_enabled=True, color=None,
            icon="call.png", text=None)),
        ('button with icon on the right', dict(
            button_name="button_iconsource_right_text", is_enabled=True,
            color=None, icon="call.png", text="Call")),
        ('button with icon on the left', dict(
            button_name="button_iconsource_left_text", is_enabled=True,
            color=None, icon="call.png", text="Call")),
        ('disabled button', dict(
            button_name="button_text_disabled", is_enabled=False, color=None,
            icon=None, text="Call"))
    ]

    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        buttons_scenarios)

    def setUp(self):
        # Reset the locale to English
        os.environ['LANGUAGE'] = 'en'
        super().setUp()

    def test_buttons(self):
        self.open_page('buttonsElement')

        button = self.app.select_single(objectName=self.button_name)
        self.assertIsNot(button, None)
        self.assertThat(button.enabled, Equals(self.is_enabled))

        if self.color is not None:
            self.assertThat(button.color, Equals(self.color))

        if self.icon is not None:
            self.assertTrue(button.iconSource.endswith(self.icon))

        if self.text is not None:
            self.assertThat(button.text, Equals(self.text))

        # try to interact with objects
        self.pointing_device.move_to_object(button)
        self.pointing_device.press()

        if button.enabled:
            self.assertThat(button.pressed, Eventually(Equals(True)))
        else:
            self.assertFalse(button.pressed)

        self.pointing_device.release()

        self.assertThat(button.pressed, Eventually(Equals(False)))
