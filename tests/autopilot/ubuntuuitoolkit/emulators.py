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

from autopilot import input, platform
from autopilot.introspection import dbus

_NO_TABS_ERROR = 'The MainView has no Tabs.'


def get_pointing_device():
    """Return the pointing device depending on the platform.

    If the platform is `Desktop`, the pointing device will be a `Mouse`.
    If not, the pointing device will be `Touch`.

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
        """Return the Header emulator of the MainView."""
        return self.select_single('Header')

    def get_toolbar(self):
        """Return the Toolbar emulator of the MainView."""
        return self.select_single(Toolbar)

    def open_toolbar(self):
        """Open the toolbar if it's not already opened.

        :return: The toolbar.

        """
        toolbar = self.get_toolbar()
        if not toolbar.opened:
            self._drag_to_open_toolbar()
            toolbar.opened.wait_for(True)

        return toolbar

    def _drag_to_open_toolbar(self):
        x, y, _, _ = self.globalRect
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
        x, y, _, _ = self.globalRect
        line_x = x + self.width * 0.50
        start_y = y + self.height * 0.95
        stop_y = y + self.height - 1

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)

    def get_tabs(self):
        """Return the Tabs emulator of the MainView."""
        tabs = self.select_single(Tabs)
        assert tabs is not None, _NO_TABS_ERROR
        return tabs

    def switch_to_next_tab(self):
        """Open the next tab.

        :return: The newly opened tab.

        """
        self.get_header().switch_to_next_tab()
        current_tab = self.get_tabs().get_current_tab()
        current_tab.visible.wait_for(True)
        return current_tab

    def switch_to_tab_by_index(self, index):
        """Open a tab.

        :parameter index: The index of the tab to open.
        :return: The newly opened tab.

        """
        tabs = self.get_tabs()
        if index >= tabs.get_number_of_tabs():
            raise IndexError('Tab index out of range.')
        current_tab = tabs.get_current_tab()
        while not tabs.selectedTabIndex == index:
            current_tab = self.switch_to_next_tab()
        return current_tab

    def switch_to_previous_tab(self):
        """Open the previous tab.

        :return: The newly opened tab.

        """
        tabs = self.get_tabs()
        if tabs.selectedTabIndex == 0:
            previous_tab_index = tabs.get_number_of_tabs() - 1
        else:
            previous_tab_index = tabs.selectedTabIndex - 1
        return self.switch_to_tab_by_index(previous_tab_index)

    def switch_to_tab(self, object_name):
        """Open a tab.

        :parameter object_name: The QML objectName property of the tab.
        :return: The newly opened tab.

        """
        tabs = self.get_tabs()
        for index, tab in enumerate(tabs.select_many('Tab')):
            if tab.objectName == object_name:
                return self.switch_to_tab_by_index(index)
        raise ValueError(
            'Tab with objectName "{0}" not found.'.format(object_name))


class Header(UbuntuUIToolkitEmulatorBase):
    """Header Autopilot emulator."""

    def __init__(self, *args):
        super(Header, self).__init__(*args)
        self.pointing_device = get_pointing_device()

    def _get_animating(self):
        tab_bar_style = self.select_single('TabBarStyle')
        return tab_bar_style.animating

    def switch_to_next_tab(self):
        """Open the next tab."""
        tab_bar = self.select_single('TabBar')
        assert tab_bar is not None, _NO_TABS_ERROR
        tab_bar_x, tab_bar_y, _, _ = tab_bar.globalRect
        line_y = tab_bar_y + tab_bar.height * 0.5
        current_tab_x = tab_bar_x + tab_bar.width * 0.35
        next_tab_x = tab_bar_x + tab_bar.width * 0.65

        self.pointing_device.move(current_tab_x, line_y)
        self.pointing_device.click()
        self.pointing_device.move(next_tab_x, line_y)
        self.pointing_device.click()

        # Sleep while the animation finishes.
        self._get_animating().wait_for(False)


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
        if button is None:
            raise ValueError(
                'Button with objectName "{0}" not found.'.format(object_name))
        self.pointing_device.click_object(button)

    def _get_button(self, object_name):
        return self.select_single('ActionItem', objectName=object_name)


class Tabs(UbuntuUIToolkitEmulatorBase):
    """Tabs Autopilot emulator."""

    def get_current_tab(self):
        """Return the currently selected tab."""
        return self.select_many('Tab')[self.selectedTabIndex]

    def get_number_of_tabs(self):
        """Return the number of tabs."""
        return len(self.select_many('Tab'))
