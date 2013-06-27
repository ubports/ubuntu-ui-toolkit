# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012, 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


from autopilot.introspection import dbus


class UbuntuUIToolkitEmulatorBase(dbus.CustomEmulatorBase):
    """A base class for all the Ubuntu UI Toolkit emulators."""


class MainView(UbuntuUIToolkitEmulatorBase):
    """MainView Autopilot emulator."""

    @property
    def header(self):
        """Get the Header emulator of the MainView."""
        return self.select_single(Header)

    def get_object(self, typeName, name):
        return self.app.select_single(typeName, objectName=name)

    def get_object_by_text(self, typeName, itemText):
        return self.app.select_single(typeName, text=itemText)


class Header(UbuntuUIToolkitEmulatorBase):
    """Header Autopilot emulator."""
