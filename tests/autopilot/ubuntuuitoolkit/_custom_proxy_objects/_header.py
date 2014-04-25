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

import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import (
    _common,
    _tabbar
)


_NO_TABS_ERROR = 'The MainView has no Tabs.'


logger = logging.getLogger(__name__)


class Header(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Header Autopilot emulator."""

    def __init__(self, *args):
        super(Header, self).__init__(*args)
        self.pointing_device = _common.get_pointing_device()

    def _get_animating(self):
        if self.useDeprecatedToolbar:
            tab_bar_style = self.select_single('TabBarStyle')
            return tab_bar_style.animating
        else:
            return False

    @autopilot_logging.log_action(logger.info)
    def switch_to_next_tab(self):
        """Open the next tab.

        :raise ToolkitEmulatorException: If the main view has no tabs.

        """
        if self.useDeprecatedToolbar:
            self._switch_to_next_tab_in_deprecated_tabbar()
        else:
            self._switch_to_next_tab_in_drawer()

    def _switch_to_next_tab_in_deprecated_tabbar(self):
        try:
            tab_bar = self.select_single(_tabbar.TabBar)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TABS_ERROR)
        tab_bar.switch_to_next_tab()
        self._get_animating().wait_for(False)

    def _switch_to_next_tab_in_drawer(self):
        tabs_model_properties = self.select_single(
            'QQuickItem', objectName='tabsModelProperties')
        next_tab_index = (tabs_model_properties.selectedIndex
                          + 1) % tabs_model_properties.count
        self._switch_to_tab_in_drawer_by_index(next_tab_index)

    @autopilot_logging.log_action(logger.info)
    def switch_to_tab_by_index(self, index):
        """Open a tab. This only supports the new tabs in the header

        :parameter index: The index of the tab to open.
        :raise ToolkitEmulatorException: If the tab index is out of range or
                useDeprecatedToolbar is set.

        """
        if self.useDeprecatedToolbar:
            raise _common.ToolkitException(
                "Header.swtich_to_tab_by_index only works with new header")
        else:
            self._switch_to_tab_in_drawer_by_index(index)

    def _switch_to_tab_in_drawer_by_index(self, index):
        try:
            tabs_drawer_button = self.select_single(
                'AbstractButton', objectName='tabsButton')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TABS_ERROR)
        self.pointing_device.click_object(tabs_drawer_button)

        tabs_model_properties = self.select_single(
            'QQuickItem', objectName='tabsModelProperties')

        if tabs_model_properties.selectedIndex == index:
            return

        try:
            tab_button = self.get_root_instance().select_single(
                'Standard', objectName='tabButton' + str(index))
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                "Tab button {0} not found.".format(index))

        self.pointing_device.click_object(tab_button)
