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


class Empty(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Base class to emulate swipe to delete."""

    def exists(self):
        try:
            return self.implicitHeight > 0
        except dbus.StateNotFoundError:
            return False

    def _get_confirm_button(self):
        return self.select_single(
            'QQuickItem', objectName='confirmRemovalDialog')

    @autopilot_logging.log_action(logger.info)
    def swipe_to_delete(self, direction='right'):
        """Swipe the item in a specific direction."""
        if self.removable:
            # Swipe to delete is always right to left
            if direction == 'left':
                direction = 'right'
            self._drag_pointing_device_to_delete(direction)
            if self.confirmRemoval:
                self.waitingConfirmationForRemoval.wait_for(True)
            else:
                self._wait_until_deleted()
        else:
            raise _common.ToolkitException(
                'The item "{0}" is not removable'.format(self.objectName))

    def _drag_pointing_device_to_delete(self, direction):
        x, y, width, height = self.globalRect
        left_x = x + (width * 0.2)
        right_x = x + (width * 0.8)
        start_y = stop_y = y + (height // 2)

        if direction == 'right':
            start_x = left_x
            stop_x = right_x
            self.pointing_device.drag(start_x, start_y, stop_x, stop_y)
        elif direction == 'left':
            start_x = right_x
            stop_x = left_x
            self.pointing_device.drag(start_x, start_y, stop_x, stop_y)
        else:
            raise _common.ToolkitException(
                'Invalid direction "{0}" used on swipe to delete function'
                .format(direction))

    def _wait_until_deleted(self):
        try:
            # The item was hidden.
            self.implicitHeight.wait_for(0)
        except dbus.StateNotFoundError:
            # The item was destroyed.
            pass

    @autopilot_logging.log_action(logger.info)
    def confirm_removal(self):
        """Comfirm item removal if this was already swiped."""
        if self.waitingConfirmationForRemoval:
            deleteButton = self._get_confirm_button()
            self.pointing_device.click_object(deleteButton)
            self._wait_until_deleted()
        else:
            raise _common.ToolkitException(
                'The item "{0}" is not waiting for removal confirmation'.
                format(self.objectName))


class Base(Empty):
    pass


class Standard(Empty):
    pass


class ItemSelector(Empty):
    pass


class SingleControl(Empty):
    pass


class MultiValue(Base):
    pass


class SingleValue(Base):
    pass


class Subtitled(Base):
    pass
