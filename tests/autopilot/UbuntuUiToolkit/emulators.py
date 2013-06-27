# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from autopilot.introspection import dbus


class UbuntuUIToolkitEmulatorBase(dbus.CustomEmulatorBase):
    """A base class for all the Ubuntu UI Toolkit emulators."""


class MainView(UbuntuUIToolkitEmulatorBase):
    """MainView Autopilot emulator."""

    def get_header(self):
        """Get the Header emulator of the MainView."""
        return self.select_single('Header')

