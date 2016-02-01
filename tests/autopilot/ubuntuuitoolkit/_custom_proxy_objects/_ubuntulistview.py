# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

from ubuntuuitoolkit._custom_proxy_objects import _qquicklistview


class UbuntuListView11(_qquicklistview.QQuickListView):
    """Autopilot helper for the UbuntuListView 1.1."""

    def pull_to_refresh_enabled(self):
        refresh = self.select_single('PullToRefresh')
        return refresh.enabled

    def manual_refresh_wait(self):
        refresh = self.select_single('PullToRefresh')
        if refresh.enabled:
            self.pull_to_refresh()
            refresh.wait_for_refresh()
            return True
        return False

    def manual_refresh_nowait(self):
        refresh = self.select_single('PullToRefresh')
        if refresh.enabled:
            self.pull_to_refresh()
            return True
        return False

    def wait_refresh_completed(self):
        refresh = self.select_single('PullToRefresh')
        if refresh.enabled:
            refresh.wait_for_refresh()
            return True
        return False


class UbuntuListView(UbuntuListView11):
    """Autopilot helper for the UbuntuListView 1.3."""
