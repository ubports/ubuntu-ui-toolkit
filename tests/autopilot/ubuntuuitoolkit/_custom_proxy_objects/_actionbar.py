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

    @autopilot_logging.log_action(logger.info)
    def click_action_button(self, action_object_name):
        """Click an action button of the action bar.

        :parameter object_name: The QML objectName property of the action
        :raise ToolkitException: If there is no action button with that object
            name.

        """

        try:
            object_name = action_object_name + "_action_button"
            button = self.select_single(objectName=object_name)
            self.pointing_device.click_object(button)
        except dbus.StateNotFoundError:
            # the button is not in the ActionBar, but it may be in the overflow
            try:
                popover = self._open_overflow()
                popover.click_action_button(action_object_name)
            except dbus.StateNotFoundError:
                raise _common.ToolkitException(
                    'Button not found in ActionBar or overflow')
