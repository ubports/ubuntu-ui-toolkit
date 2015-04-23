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
        buttons = self.select_many('AbstractButton')
        texts = []
        for button in buttons:
            # workaround used in the text input's context menu to access
            # action.text so we can get the proper button by text, action
            # being not accessible
            # https://bugs.launchpad.net/autopilot/+bug/1334599
            if button.text == text:
                return button
            texts.append(button.text)
        raise _common.ToolkitException(
            'Could not find a button with text %s (Available buttons are %s)'
            % (text, ','.join(texts)))


class ActionSelectionPopover(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """ActionSelectionPopover Autopilot custom proxy object."""

    def click_button_by_text(self, text):
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
