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
from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class UCListItem(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Base class to emulate swipe for leading and trailing actions."""

    @autopilot_logging.log_action(logger.info)
    def _swipe_in_panel(self, panel_item):
        """ Swipe in panel (leading/trailing)"""
        x, y, width, height = self.globalRect
        if panel_item == 'leading':
            start_x = x + (width * 0.2)
            stop_x = x + (width * 0.8)
        elif panel_item == 'trailing':
            start_x = x + (width * 0.8)
            stop_x = x + (width * 0.2)
        else:
            raise _common.ToolkitException(
                'No {0} panel found in a ListItem'.format(panel_item))
        start_y = stop_y = y + (height // 2)
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y)

    def _click_on_panel_action(self, panel_item, action_object, wait_function):
        self._swipe_in_panel(panel_item)
        try:
            button_name = 'actionbutton_' + action_object
            action_button = self.select_single(objectName=button_name)
        except:
            raise _common.ToolkitException(
                'The requested action not found on {0} side'.
                format(panel_item))

        self.pointing_device.click_object(action_button)
        if wait_function is None:
            # wait for the animation to finish
            contentItem = self.select_single(objectName='ListItemHolder')
            contentItem.x.wait_for(0)
        else:
            wait_function()

    @autopilot_logging.log_action(logger.info)
    def trigger_leading_action(self, action_objectName, wait_function=None):
        """Swipe the item in from left to right to open leading actions
           and click on the button representing the requested action.

           parameters: action_objectName - object name of the action to be
                       triggered.
                       wait_function - a custom wait function to wait till the
                       action is triggered
        """
        self._click_on_panel_action('leading', action_objectName,
                                    wait_function)

    @autopilot_logging.log_action(logger.info)
    def trigger_trailing_action(self, action_objectName, wait_function=None):
        """Swipe the item in from right to left to open trailing actions
           and click on the button representing the requested action.

           parameters: action_objectName - object name of the action to be
                       triggered.
                       wait_function - a custom wait function to wait till the
                       action is triggered
        """
        self._click_on_panel_action('trailing', action_objectName,
                                    wait_function)

    @autopilot_logging.log_action(logger.info)
    def toggle_selected(self):
        """Toggles selected state of the ListItem. """
        toggle = self.select_single(objectName='listitem_select')
        self.pointing_device.click_object(toggle)
