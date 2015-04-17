# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014, 2015 Canonical Ltd.
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

import logging
from distutils import version

import autopilot
from autopilot import (
    input,
    logging as autopilot_logging,
    platform
)
from autopilot.introspection import dbus


logger = logging.getLogger(__name__)


class ToolkitException(Exception):
    """Exception raised when there is an error with the custom proxy object."""


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
        match the required by the custom proxy objects.

    """
    installed_version = version.LooseVersion(autopilot.version)
    if installed_version < version.LooseVersion('1.4'):
        raise ToolkitException(
            'The custom proxy objects require Autopilot 1.4 or higher.')


class UbuntuUIToolkitCustomProxyObjectBase(dbus.CustomEmulatorBase):
    """A base class for all the Ubuntu UI Toolkit custom proxy objects."""

    def __init__(self, *args):
        check_autopilot_version()
        super().__init__(*args)
        self.pointing_device = get_pointing_device()

    def is_flickable(self):
        """Check if the object is flickable.

        If the object has a flicking attribute, we consider it as a flickable.

        :return: True if the object is flickable. False otherwise.

        """
        try:
            self.flicking
            return True
        except AttributeError:
            return False

    @autopilot_logging.log_action(logger.info)
    def swipe_into_view(self):
        """Make the object visible.

        Currently it works only when the object needs to be swiped vertically.
        TODO implement horizontal swiping. --elopio - 2014-03-21

        """
        flickable_parent = self._get_flickable_parent()
        flickable_parent.swipe_child_into_view(self)

    def _get_flickable_parent(self):
        parent = self.get_parent()
        root = self.get_root_instance()
        while parent.id != root.id:
            if parent.is_flickable():
                return parent
            parent = parent.get_parent()
        raise ToolkitException(
            "The element is not contained in a Flickable so it can't be "
            "swiped into view.")

    def _get_top_container(self):
        """Return the top-most container with a globalRect."""
        root = self.get_root_instance()
        parent = self.get_parent()
        top_container = None
        while parent.id != root.id:
            if hasattr(parent, 'globalRect'):
                top_container = parent

            parent = parent.get_parent()

        if top_container is None:
            raise ToolkitException('Could not find the top-most container.')
        else:
            return top_container
