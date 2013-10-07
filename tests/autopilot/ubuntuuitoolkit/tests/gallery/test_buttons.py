# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
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

import os
import shutil

from autopilot.matchers import Eventually
from testtools.matchers import Is, Not, Equals
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class ButtonsTestCase(GalleryTestCase):

    scenarios = [
        ('standard button', dict(
            button_name="button_text", is_enabled=True, color=None, icon=None,
            text="Call")),
        ('button with color', dict(
            button_name="button_color", is_enabled=True,
            color=[0, 0, 0, 255], icon=None, text="Call")),
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

    def test_buttons(self):
        item = "Buttons"
        self.loadItem(item)
        self.checkPageHeader(item)

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
