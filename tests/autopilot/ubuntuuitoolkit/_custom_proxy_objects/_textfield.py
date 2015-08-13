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

from ubuntuuitoolkit._custom_proxy_objects import _common, _mainview

logger = logging.getLogger(__name__)


class TextField(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """TextField Autopilot custom proxy object."""

    def __init__(self, *args):
        super().__init__(*args)
        self.keyboard = _common.get_keyboard()

    @autopilot_logging.log_action(logger.info)
    def write(self, text, clear=True):
        """Write into the text field.

        :parameter text: The text to write.
        :parameter clear: If True, the text field will be cleared before
            writing the text. If False, the text will be appended at the end
            of the text field. Default is True.

        """
        self._ensure_focused()
        if clear:
            self.clear()
        else:
            if not self.is_empty():
                self._go_to_end()
        self.keyboard.type(text)

    @autopilot_logging.log_action(logger.info)
    def clear(self):
        """Clear the text field."""
        self._ensure_focused()
        if not self.is_empty():
            if self.hasClearButton:
                self._click_clear_button()
            else:
                self._clear_with_keys()
            self.text.wait_for('')

    def is_empty(self):
        """Return True if the text field is empty. False otherwise."""
        return self.text == ''

    @autopilot_logging.log_action(logger.debug)
    def _click_clear_button(self):
        clear_button = self.select_single(
            'AbstractButton', objectName='clear_button')
        if not clear_button.visible:
            self.pointing_device.click_object(self)
        self.pointing_device.click_object(clear_button)

    @autopilot_logging.log_action(logger.debug)
    def _clear_with_keys(self):
        self._select_all()
        self.keyboard.press_and_release('\b')
        if not self.is_empty():
            raise _common.ToolkitException('Failed to clear the text field.')

    @autopilot_logging.log_action(logger.debug)
    def _select_all(self):
        if not self._is_all_text_selected():
            if self._is_keyboard_osk():
                cursor = self.select_single('TextCursor', visible=True)
                self.pointing_device.click_object(
                    cursor, time_between_events=0.5)
                self.pointing_device.click_object(
                    cursor, time_between_events=0.5)
                root = self.get_root_instance()
                main_view = root.select_single(_mainview.MainView)
                popover = main_view.get_text_input_context_menu(
                    'text_input_contextmenu')
                popover.click_option_by_text('Select All')
            else:
                self._go_to_start()
                self.keyboard.press('Shift')
                self._go_to_end()
                self.keyboard.release('Shift')

    def _is_all_text_selected(self):
        return self.text == self.selectedText

    def _is_keyboard_osk(self):
        """Return True if the keyboard instance is the OSK."""
        return _common.is_maliit_process_running()

    @autopilot_logging.log_action(logger.debug)
    def _go_to_end(self):
        if self._is_keyboard_osk():
            from ubuntu_keyboard.emulators.keyboard import Keyboard
            Keyboard().send_end_key()
        else:
            self.keyboard.press_and_release('End')

    @autopilot_logging.log_action(logger.debug)
    def _go_to_start(self):
        if self._is_keyboard_osk():
            from ubuntu_keyboard.emulators.keyboard import Keyboard
            Keyboard().send_home_key()
        else:
            self.keyboard.press_and_release('Home')

    @autopilot_logging.log_action(logger.debug)
    def _ensure_focused(self):
        if not self.activeFocus:
            self.pointing_device.click_object(self)
            self.activeFocus.wait_for(True)
