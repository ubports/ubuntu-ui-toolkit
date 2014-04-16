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

from ubuntuuitoolkit._custom_proxy_objects import (
    _common,
    _tabbar
)


_NO_TABS_ERROR = 'The MainView has no Tabs.'


logger = logging.getLogger(__name__)


class Header(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Header Autopilot emulator."""

    def __init__(self, *args):
        super(Header, self).__init__(*args)
        self.pointing_device = _common.get_pointing_device()

    def _get_animating(self):
        tab_bar_style = self.select_single('TabBarStyle')
        return tab_bar_style.animating

    @autopilot_logging.log_action(logger.info)
    def switch_to_next_tab(self):
        """Open the next tab.

        :raise ToolkitException: If the main view has no tabs.

        """
        try:
            tab_bar = self.select_single(_tabbar.TabBar)
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(_NO_TABS_ERROR)
        tab_bar.switch_to_next_tab()
        self._get_animating().wait_for(False)
