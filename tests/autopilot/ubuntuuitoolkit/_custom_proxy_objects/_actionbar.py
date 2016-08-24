# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2015 Canonical Ltd.
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
from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class ActionBar(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """ActionBar Autopilot custom proxy object."""

    def __init__(self, *args):
        super().__init__(*args)
        # listview will only be set for a scrolling ActionBar.
        try:
            self.listview = self.select_single(objectName='actions_listview')
        except dbus.StateNotFoundError:
            self.listview = ''

    def _open_overflow(self):
        """Click the overflow button and return the overflow panel"""
        actions_overflow_button = self.select_single(
            objectName='overflow_action_button')

        if not actions_overflow_button.visible:
            raise _common.ToolkitException('No actions in overflow')

        # open the popover
        self.pointing_device.click_object(actions_overflow_button)

        # the popover is not a child of the ActionBar, so use the popover
        # object to find the requested button
        try:
            popover = self.get_root_instance().select_single(
                objectName='actions_overflow_panel')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                'Failed to select overflow panel')

        return popover

    def _overflow_bar_click_action_button(self, action_object_name):
        try:
            object_name = action_object_name + "_button"
            button = self.select_single(objectName=object_name)
            # In an animating header, the button is disabled until the header
            #   animation is done. Wait for that:
            button.enabled.wait_for(True)
            self.pointing_device.click_object(button)
        except dbus.StateNotFoundError:
            # the button is not in the ActionBar, but it may be in the overflow
            try:
                popover = self._open_overflow()
                popover.click_action_button(action_object_name)
            except _common.ToolkitException:
                raise _common.ToolkitException(
                    'Button not found')

    def _scrolling_bar_click_action_button(self, action_object_name):
        rightMessage = str("Can't swipe more, we are already at the " +
                           "right of the container.")
        leftMessage = str("Can't swipe more, we are already at the " +
                          "left of the container.")
        buttonName = action_object_name + "_button"
        try:
            self.listview.click_element(buttonName)
        except _common.ToolkitException as e:
            if (e.args[0] == leftMessage or e.args[0] == rightMessage):
                # The button was found, but is not inside the ListView. This
                # happens because at the beginning and end of the ListView
                # there are list items visible inside the extra margins. But
                # the buttons are present otherwise a different error message
                # wass given.
                button = self.listview.select_single(objectName=buttonName)
                self.pointing_device.click_object(button)
            else:
                raise _common.ToolkitException(
                    'Button not found')

    @autopilot_logging.log_action(logger.info)
    def click_action_button(self, action_object_name):
        """Click an action button of the action bar.

        :parameter object_name: The QML objectName property of the action
        :raise ToolkitException: If there is no action button with that object
            name.

        """
        if self.styleName == "ActionBarStyle":
            return self._overflow_bar_click_action_button(action_object_name)
        elif self.styleName == "ScrollingActionBarStyle":
            return self._scrolling_bar_click_action_button(action_object_name)
        else:
            raise _common.ToolkitException(
                'Unsupported style name ' + self.styleName + ' for ActionBar.')
