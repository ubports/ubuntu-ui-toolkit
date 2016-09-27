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
from autopilot import introspection

from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class Dialog(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Autopilot helper for the Dialog component."""


class TextInputPopover(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """TextInputPopover Autopilot emulator ."""

    def click_option_by_text(self, text):
        """Click a button on the popover.

        XXX We are receiving the text because there's no way to set the
        objectName on the action. This is reported at
        https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205144
        --elopio - 2013-07-25

        :parameter text: The text of the button.
        :raise ToolkitException: If the popover is not open.

        """
        if not self.visible:
            raise _common.ToolkitException('The popover is not open.')
        button = self._get_button(text)
        if button is None:
            raise _common.ToolkitException(
                'Button with text "{0}" not found.'.format(text))
        self.pointing_device.click_object(button)
        if self.autoClose:
            try:
                self.visible.wait_for(False)
            except dbus.StateNotFoundError:
                # The popover was removed from the tree.
                pass

    def _get_button(self, text):
        # Try the C++ class name in case this is a 1.3 AbstractButton
        try:
            return self.wait_select_single(
                'UbuntuToolkit::UCAbstractButton', text=text)
        except dbus.StateNotFoundError:
            return self.wait_select_single('AbstractButton', text=text)


class ActionSelectionPopover(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """ActionSelectionPopover Autopilot custom proxy object."""

    @classmethod
    def validate_dbus_object(cls, path, state):
        if super().validate_dbus_object(path, state):
            return True

        name = introspection.get_classname_from_path(path)
        if name == b'OverflowPanel':
            return True

        return False

    def click_action_button(self, action_object_name):
        """Click an action button on the popover.

        :parameter object_name: The QML objectName property of the action
        :raise ToolkitException: If there is no visible button with that object
            name or the popover is not open.

        """

        if not self.visible:
            raise _common.ToolkitException('The popover is not open.')
        try:
            object_name = action_object_name + "_button"
            button = self.select_single(objectName=object_name)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                'Button for action with objectName "{0}" not found.'.format(
                    action_object_name))
        if not button.visible:
            raise _common.ToolkitException(
                'Button for action with objectName "{0}" not visible.'.format(
                    action_object_name))
        self.pointing_device.click_object(button)
        if self.autoClose:
            try:
                self.visible.wait_for(False)
            except dbus.StateNotFoundError:
                # The popover was removed from the tree.
                pass

    def click_button_by_text(self, text):
        """Click a button on the popover.

        :parameter text: The text of the button.
        :raise ToolkitException: If there is no visible button with that label
            or the popover is not open.

        """
        if not self.visible:
            raise _common.ToolkitException('The popover is not open.')
        button = self._get_button(text)
        if button is None:
            raise _common.ToolkitException(
                'Button with text "{0}" not found.'.format(text))
        self.pointing_device.click_object(button)
        if self.autoClose:
            try:
                self.visible.wait_for(False)
            except dbus.StateNotFoundError:
                # The popover was removed from the tree.
                pass

    def _get_button(self, text):
        buttons = self.select_many('Empty')
        for button in buttons:
            if button.text == text:
                return button


class ComposerSheet(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """ComposerSheet Autopilot custom proxy object."""

    @autopilot_logging.log_action(logger.info)
    def confirm(self):
        """Confirm the composer sheet."""
        button = self.select_single('Button', objectName='confirmButton')
        self.pointing_device.click_object(button)
        self.wait_until_destroyed()

    @autopilot_logging.log_action(logger.info)
    def cancel(self):
        """Cancel the composer sheet."""
        button = self.select_single('Button', objectName='cancelButton')
        self.pointing_device.click_object(button)
        self.wait_until_destroyed()
