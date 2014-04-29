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

"""Common helpers for Ubuntu UI Toolkit Autopilot custom proxy objects."""

from distutils import version

import autopilot
from autopilot import platform, input
from autopilot.introspection import dbus


class ToolkitException(Exception):
    """Exception raised when there is an error with the emulator."""


def get_pointing_device():
    """Return the pointing device depending on the platform.

    If the platform is `Desktop`, the pointing device will be a `Mouse`.
    If not, the pointing device will be `Touch`.

    """
    if platform.model() == 'Desktop':
        input_device_class = input.Mouse
    else:
        input_device_class = input.Touch
    return input.Pointer(device=input_device_class.create())


def get_keyboard():
    """Return the keyboard device."""
    # TODO return the OSK if we are on the phone. --elopio - 2014-01-13
    return input.Keyboard.create()


def check_autopilot_version():
    """Check that the Autopilot installed version matches the one required.

    :raise ToolkitException: If the installed Autopilot version does't
        match the required by the emulators.

    """
    installed_version = version.LooseVersion(autopilot.version)
    if installed_version < version.LooseVersion('1.4'):
        raise ToolkitException(
            'The emulators need Autopilot 1.4 or higher.')


class UbuntuUIToolkitCustomProxyObjectBase(dbus.CustomEmulatorBase):
    """A base class for all the Ubuntu UI Toolkit emulators."""

    def __init__(self, *args):
        check_autopilot_version()
        super(UbuntuUIToolkitCustomProxyObjectBase, self).__init__(*args)
        self.pointing_device = get_pointing_device()
