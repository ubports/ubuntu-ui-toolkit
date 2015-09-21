# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014, 2015 Canonical Ltd.
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

"""Ubuntu UI Toolkit Autopilot custom proxy objects."""

import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus
from autopilot import introspection

from ubuntuuitoolkit._custom_proxy_objects import (
    AppHeader,
    _common,
    popups,
    _tabs,
    _toolbar,
)


_NO_TABS_ERROR = 'The MainView has no Tabs.'
_NO_TOOLBAR_ERROR = 'The MainView has no Toolbar.'


logger = logging.getLogger(__name__)


class MainView(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """MainView Autopilot custom proxy object."""

    def __init__(self, *args):
        super().__init__(*args)
        if not hasattr(self, 'useDeprecatedToolbar'):
            self.useDeprecatedToolbar = None

    @classmethod
    def validate_dbus_object(cls, path, state):
        if super().validate_dbus_object(path, state):
            # This covers MainView 0.1/1.0/1.1 and possible components
            # derived from MainView (e.g. "CustomMainView") that have
            # their own CPO.
            # Using objectName for selecting a MainView is recommneded.
            return True

        name = introspection.get_classname_from_path(path)
        if name == b'MainView12':
            # MainView 1.2. Must be selected using objectName.
            return True

        return False

    def get_header(self):
        """Return the AppHeader custom proxy object of the MainView."""
        try:
            return self.select_single(AppHeader, objectName='MainView_Header')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException('The main view has no header.')

    def get_toolbar(self):
        """Return the Toolbar custom proxy object of the MainView.

        :raise ToolkitException: If the main view has no toolbar.

        """
        try:
            return self.select_single(_toolbar.Toolbar)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TOOLBAR_ERROR)

    @autopilot_logging.log_action(logger.info)
    def open_toolbar(self):
        """Open the toolbar if it is not already opened.

        :return: The toolbar.
        :raise ToolkitException: If the main view has no toolbar.

        """
        return self.get_toolbar().open()

    @autopilot_logging.log_action(logger.info)
    def close_toolbar(self):
        """Close the toolbar if it is opened.

        :raise ToolkitException: If the main view has no toolbar.

        """
        self.get_toolbar().close()

    def get_tabs(self):
        """Return the Tabs custom proxy object of the MainView.

        :raise ToolkitException: If the main view has no tabs.

        """
        try:
            return self.select_single(_tabs.Tabs)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TABS_ERROR)

    @autopilot_logging.log_action(logger.info)
    def switch_to_next_tab(self):
        """Open the next tab.

        :return: The newly opened tab.

        """
        logger.debug('Switch to next tab.')
        self.get_header().switch_to_next_tab()
        current_tab = self.get_tabs().get_current_tab()
        current_tab.visible.wait_for(True)
        return current_tab

    @autopilot_logging.log_action(logger.info)
    def switch_to_tab_by_index(self, index):
        """Open a tab.

        :parameter index: The index of the tab to open.
        :return: The newly opened tab.
        :raise ToolkitException: If the tab index
                is out of range.

        """
        if self.useDeprecatedToolbar:
            return self._switch_to_tab_in_deprecated_tabbar_by_index(index)
        else:
            return self._switch_to_tab_in_drawer_by_index(index)

    def _switch_to_tab_in_deprecated_tabbar_by_index(self, index):
        tabs = self.get_tabs()
        number_of_tabs = tabs.get_number_of_tabs()
        if index >= number_of_tabs:
            raise _common.ToolkitException('Tab index out of range.')

        current_tab = tabs.get_current_tab()
        number_of_switches = 0
        while not tabs.selectedTabIndex == index:
            logger.debug(
                'Current tab index: {0}.'.format(tabs.selectedTabIndex))
            if number_of_switches >= number_of_tabs - 1:
                # This prevents a loop. But if this error is ever raised, it's
                # likely there's a bug on the helper or on the QML Tab.
                raise _common.ToolkitException(
                    'The tab with index {0} was not selected.'.format(index))
            current_tab = self.switch_to_next_tab()
            number_of_switches += 1
        return current_tab

    def _switch_to_tab_in_drawer_by_index(self, index):
        tabs = self.get_tabs()
        number_of_tabs = tabs.get_number_of_tabs()
        if index >= number_of_tabs:
            raise _common.ToolkitException('Tab index out of range.')

        if index != tabs.selectedTabIndex:
            self.get_header().switch_to_tab_by_index(index)
        current_tab = tabs.get_current_tab()
        return current_tab

    @autopilot_logging.log_action(logger.info)
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

    @autopilot_logging.log_action(logger.info)
    def switch_to_tab(self, object_name):
        """Open a tab.

        :parameter object_name: The QML objectName property of the tab.
        :return: The newly opened tab.
        :raise ToolkitException: If there is no tab with that object
            name.

        """
        tabs = self.get_tabs()
        for index, tab in enumerate(tabs.select_many('Tab')):
            if tab.objectName == object_name:
                return self.switch_to_tab_by_index(tab.index)
        raise _common.ToolkitException(
            'Tab with objectName "{0}" not found.'.format(object_name))

    def get_action_selection_popover(self, object_name):
        """Return an ActionSelectionPopover custom proxy object.

        :parameter object_name: The QML objectName property of the popover.

        """
        return self.wait_select_single(
            popups.ActionSelectionPopover, objectName=object_name)

    def get_text_input_context_menu(self, object_name):
        """Return a TextInputContextMenu emulator.

        :parameter object_name: The QML objectName property of the popover.

        """
        return self.wait_select_single(
            popups.TextInputPopover, objectName=object_name)

    @autopilot_logging.log_action(logger.info)
    def go_back(self):
        """Go to the previous page."""
        if self.useDeprecatedToolbar:
            toolbar = self.open_toolbar()
            toolbar.click_back_button()
        else:
            self.get_header().click_back_button()

    @autopilot_logging.log_action(logger.info)
    def click_action_button(self, action_object_name):
        """Click the specified button.

        :parameter action_object_name: the objectName of the action to trigger.
        :raise ToolkitException: The requested button is not available.

        """
        if self.useDeprecatedToolbar:
            raise _common.ToolkitException(
                "App is using deprecated toolbar instead of new header")
        else:
            header = self.open_header()
            header.click_action_button(action_object_name)
