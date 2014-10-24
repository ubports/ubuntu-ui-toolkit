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
    def swipe_to_delete(self, direction='deprecated'):
        """Swipe the item in from left to right in order to delete it.

        :param direction: deprecated. The only direction that will cause a
            delete is left to right. Do not pass any argument to this method,
            because it will be removed in future versions.

        """
        if direction != 'deprecated':
            logger.warn(
                'The direction argument is deprecated. Now the Ubuntu SDK '
                'only deletes list items when swiping from left to right. '
                'Call swipe_to_delete without arguments.')
        else:
            direction = 'right'
        if self.removable:
            self._swipe_to_delete_removable_item(direction)
        else:
            raise _common.ToolkitException(
                'The item "{0}" is not removable'.format(self.objectName))

    def _swipe_to_delete_removable_item(self, direction):
        if direction == 'left':
            raise _common.ToolkitException(
                'Only swiping to the right will cause the item to be deleted.')
        elif direction != 'right':
            raise _common.ToolkitException(
                'Invalid direction "{0}" used on swipe to delete function'
                .format(direction))

        self._drag_pointing_device_to_delete()
        if self.confirmRemoval:
            self.waitingConfirmationForRemoval.wait_for(True)
        else:
            self._wait_until_deleted()

    def _drag_pointing_device_to_delete(self):
        x, y, width, height = self.globalRect
        start_x = x + (width * 0.2)
        stop_x = x + (width * 0.8)
        start_y = stop_y = y + (height // 2)

        self.pointing_device.drag(start_x, start_y, stop_x, stop_y)

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


class Expandable(Empty):

    @autopilot_logging.log_action(logger.info)
    def expand(self):
        """Expand an expandable list item.

        If the item is already expanded, no action will be executed.
        If you want to check if the item is expanded, you can use the
        ``expanded`` property.

        """
        if not self.expanded:
            self._click_always_visible_section()
            self.expanded.wait_for(True)
            self.height.wait_for(self.expandedHeight)
        else:
            logger.debug('The item is already expanded.')

    @autopilot_logging.log_action(logger.debug)
    def _click_always_visible_section(self):
        self.pointing_device.move(
            self.globalRect.x + self.globalRect.width // 2,
            self.globalRect.y + self.collapsedHeight // 2)
        self.pointing_device.click()

    @autopilot_logging.log_action(logger.info)
    def collapse(self):
        """Collapse an expandable list item.

        If the item is already collapsed, no action will be executed.
        If you want to check if the item is expanded, you can use the
        ``expanded`` property.

        """
        if self.expanded:
            self._click_always_visible_section()
            self.expanded.wait_for(False)
            self.height.wait_for(self.collapsedHeight)
        else:
            logger.debug('The item is already collapsed.')


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
