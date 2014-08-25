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

from ubuntuuitoolkit._custom_proxy_objects import _common


class Tabs(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Tabs Autopilot custom proxy object."""

    def get_current_tab(self):
        """Return the currently selected tab."""
        return self._get_tab(self.selectedTabIndex)

    def _get_tab(self, index):
        tabs = self._get_tabs()
        for tab in tabs:
            if tab.index == index:
                return tab
        else:
            raise _common.ToolkitException(
                'There is no tab with index {0}.'.format(index))

    def _get_tabs(self):
        return self.select_many('Tab')

    def get_number_of_tabs(self):
        """Return the number of tabs."""
        return len(self._get_tabs())
