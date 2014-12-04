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

from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)

class UCListItem(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Base class to emulate swipe for leading and trailing actions."""

    @autopilot_logging.log_action(logger.info)
    def _swipe_content(self, direction):
        x, y, width, height = self.globalRect
        if direction == 'right':
            start_x = x + (width * 0.2)
            stop_x = x + (width * 0.8)
        else:
            start_x = x + (width * 0.8)
            stop_x = x + (width * 0.2)
        start_y = stop_y = y + (height // 2)
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y)

    def _click_on_panel_action(self, swipe_direction, action_object):
        self._swipe_content(swipe_direction)
        try:
            button_name = 'actionbutton_' + action_object
            action_button = self.select_single(objectName=button_name)
        except dbus.StateNotFound:
            raise _common.ToolkitException(
                'The requested action not found on leading side')

        self.pointing_device.click_object(action_button)
        # wait for the animation to finish, contentItem must be 0
        contentItem = self.select_single(objectName='ListItemHolder')
        contentItem.x.wait_for(0)

    @autopilot_logging.log_action(logger.info)
    def click_leading_action(self, action_objectName):
        """Swipe the item in from left to right to open leading actions."""
        self._click_on_panel_action('right', action_objectName)

    @autopilot_logging.log_action(logger.info)
    def click_trailing_action(self, action_objectName):
        """Swipe the item in from right to left to open trailing actions."""
        self._click_on_panel_action('left', action_objectName)
