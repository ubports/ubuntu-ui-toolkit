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
        # FIXME TIM: clean this up.
#        self.listview = ''
#        if self.styleName == "ScrollingActionBar":
        # Note that listview will only be set for a scrolling ActionBar.
        self.listview = self.select_single(objectName='actions_listview')

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
            self.pointing_device.click_object(button)
        except dbus.StateNotFoundError:
            # the button is not in the ActionBar, but it may be in the overflow
            try:
                popover = self._open_overflow()
                popover.click_action_button(action_object_name)
            except _common.ToolkitException:
                raise _common.ToolkitException(
                    'Button not found in ActionBar or overflow')

    def _scrolling_bar_click_action_button(self, action_object_name):
        rightMessage = "Can't swipe more, we are already at the right of the container."
        leftMessage = "Can't swipe more, we are already at the left of the container."
        try:
            self.listview.click_element(action_object_name + "_button")
        except _common.ToolkitException as e:
            # TODO: Deal with the special case where the scroll button is not
            #   visible and the button is visible within the Rectangle of the
            #   ScrollingActionBarStyle background.
            if (e.args[0] == leftMessage):
                raise e
            elif (e.args[0] == rightMessage):
                raise e
            else:
                raise _common.ToolkitException(
                    'Button with objectName ' + action_object_name +
                    ' not found in scrolling ActionBar.')


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
