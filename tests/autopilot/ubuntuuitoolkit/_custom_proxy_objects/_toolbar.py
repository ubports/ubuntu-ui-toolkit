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


class Toolbar(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Toolbar Autopilot custom proxy object."""

    @autopilot_logging.log_action(logger.info)
    def open(self):
        """Open the toolbar if it's not already opened.

        :return: The toolbar.

        """
        self.animating.wait_for(False)
        if not self.opened:
            self._drag_to_open()
            self.opened.wait_for(True)
            self.animating.wait_for(False)

        return self

    def _drag_to_open(self):
        x, y, _, _ = self.globalRect
        line_x = x + self.width * 0.50
        start_y = y + self.height - 1
        stop_y = y

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)

    @autopilot_logging.log_action(logger.info)
    def close(self):
        """Close the toolbar if it's opened."""
        self.animating.wait_for(False)
        if self.opened:
            self._drag_to_close()
            self.opened.wait_for(False)
            self.animating.wait_for(False)

    def _drag_to_close(self):
        x, y, _, _ = self.globalRect
        line_x = x + self.width * 0.50
        start_y = y
        stop_y = y + self.height - 1

        self.pointing_device.drag(line_x, start_y, line_x, stop_y)

    @autopilot_logging.log_action(logger.info)
    def click_button(self, object_name):
        """Click a button of the toolbar.

        The toolbar should be opened before clicking the button, or an
        exception will be raised. If the toolbar is closed for some reason
        (e.g., timer finishes) after moving the mouse cursor and before
        clicking the button, it is re-opened automatically by this function.

        :parameter object_name: The QML objectName property of the button.
        :raise ToolkitException: If there is no button with that object
            name.

        """
        # ensure the toolbar is open
        if not self.opened:
            raise _common.ToolkitException(
                'Toolbar must be opened before calling click_button().')
        try:
            button = self._get_button(object_name)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                'Button with objectName "{0}" not found.'.format(object_name))
        self.pointing_device.move_to_object(button)
        # ensure the toolbar is still open (may have closed due to timeout)
        self.open()
        # click the button
        self.pointing_device.click_object(button)

    def _get_button(self, object_name):
        return self.select_single(objectName=object_name)

    @autopilot_logging.log_action(logger.info)
    def click_back_button(self):
        """Click the back button of the toolbar."""
        self.click_button('back_toolbar_button')
