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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from autopilot import input, platform
from autopilot.introspection import dbus


def get_pointing_device():
    """Get the pointing device depending on the platform.

    If the platform is `Desktop`, the pointing device will be a `Mouse`.
    If it is `Touch`, the pointing device will be `Touch`.

    """
    if platform.model == 'Desktop':
        input_device_class = input.Mouse
    else:
        input_device_class = input.Touch
    return input.Pointer(input_device_class.create())


class UbuntuUIToolkitEmulatorBase(dbus.CustomEmulatorBase):
    """A base class for all the Ubuntu UI Toolkit emulators."""


class MainView(UbuntuUIToolkitEmulatorBase):
    """MainView Autopilot emulator."""

    def __init__(self, *args):
        super(MainView, self).__init__(*args)
        self.pointing_device = get_pointing_device()

    def get_header(self):
        """Get the Header emulator of the MainView."""
        return self.select_single('Header')

    def get_toolbar(self):
        """Get the Toolbar emulator of the MainView."""
        return self.select_single(Toolbar)

    def open_toolbar(self):
        """Open the toolbar if it's not already opened.

        :return: the toolbar.

        """
        toolbar = self.get_toolbar()
        if not toolbar.opened:
            self._drag_to_open_toolbar()
            toolbar.opened.wait_for(True)

        return toolbar

    def _drag_to_open_toolbar(self):
        x = self.globalRect[0]
        y = self.globalRect[1]
        line_x = x + self.width * 0.50
        start_y = y + self.height - 1
        stop_y = y + self.height * 0.95

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)

    def close_toolbar(self):
        """Close the toolbar if it's opened."""
        toolbar = self.get_toolbar()
        if toolbar.opened:
            self._drag_to_close_toolbar()
            toolbar.opened.wait_for(False)

    def _drag_to_close_toolbar(self):
        x = self.globalRect[0]
        y = self.globalRect[1]
        line_x = x + self.width * 0.50
        start_y = y + self.height * 0.95
        stop_y = y + self.height - 1

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)


class Toolbar(UbuntuUIToolkitEmulatorBase):
    """Toolbar Autopilot emulator."""

    def __init__(self, *args):
        super(Toolbar, self).__init__(*args)
        self.pointing_device = get_pointing_device()

    def click_button(self, object_name):
        """Click a button of the toolbar.

        :param object_name: The QML objectName property of the button.

        """
        button = self._get_button(object_name)
        not_found_error = 'Button with objectName "{0}" not found.'.format(
            object_name)
        assert button is not None, not_found_error
        self.pointing_device.click_object(button)

    def _get_button(self, object_name):
        return self.select_single('ActionItem', objectName=object_name)
