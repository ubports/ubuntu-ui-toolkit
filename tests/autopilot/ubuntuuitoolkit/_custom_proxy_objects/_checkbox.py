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


class CheckBox(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """CheckBox Autopilot custom proxy object."""

    @autopilot_logging.log_action(logger.info)
    def check(self, timeout=10):
        """Check a CheckBox, if its not already checked.

        :parameter timeout: number of seconds to wait for the CheckBox to be
            checked. Default is 10.

        """
        if not self.checked:
            self.change_state(timeout)

    @autopilot_logging.log_action(logger.info)
    def uncheck(self, timeout=10):
        """Uncheck a CheckBox, if its not already unchecked.

        :parameter timeout: number of seconds to wait for the CheckBox to be
            unchecked. Default is 10.

        """
        if self.checked:
            self.change_state(timeout)

    @autopilot_logging.log_action(logger.info)
    def change_state(self, timeout=10):
        """Change the state of a CheckBox.

        If it is checked, it will be unchecked. If it is unchecked, it will be
        checked.

        :parameter time_out: number of seconds to wait for the CheckBox state
            to change. Default is 10.

        """
        original_state = self.checked
        self.pointing_device.click_object(self)
        self.checked.wait_for(not original_state, timeout)
