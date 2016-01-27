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
import subprocess
import time
from distutils import version
from gi.repository import Gio

import autopilot
from autopilot import (
    input,
    logging as autopilot_logging,
    platform
)
from autopilot.introspection import dbus

logger = logging.getLogger(__name__)

MALIIT = 'maliit-server'


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
    if is_maliit_process_running():
        configure_osk_settings()
        restart_maliit_with_testability()
        return input.Keyboard.create('OSK')
    else:
        return input.Keyboard.create()


def restart_maliit_with_testability():
    """Restart maliit-server with testability enabled."""
    pid = get_process_pid(MALIIT)
    if _is_testability_enabled_for_process(pid):
        return
    _stop_process(MALIIT)
    _start_process(MALIIT, 'QT_LOAD_TESTABILITY=1')
    # This is needed to work around https://launchpad.net/bugs/1248913
    time.sleep(5)


def configure_osk_settings():
    """Configure OSK ready for testing by turning off all helpers."""
    gsettings = Gio.Settings.new("com.canonical.keyboard.maliit")
    gsettings.set_string("active-language", "en")
    gsettings.set_boolean("auto-capitalization", False)
    gsettings.set_boolean("auto-completion", False)
    gsettings.set_boolean("predictive-text", False)
    gsettings.set_boolean("spell-checking", False)
    gsettings.set_boolean("double-space-full-stop", False)


def _is_testability_enabled_for_process(pid):
    """Return True if testability is enabled for specified process."""
    proc_env = '/proc/{pid}/environ'.format(pid=pid)
    command = ['cat', proc_env]
    try:
        output = subprocess.check_output(
            command,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
        )
    except subprocess.CalledProcessError as e:
        e.args += ('Failed to get environment for pid {}: {}.'.format(
            pid, e.output),)
        raise
    return output.find('QT_LOAD_TESTABILITY=1') >= 0


def _stop_process(proc_name):
    """Stop process with name proc_name."""
    logger.info('Stoping process {}.'.format(proc_name))
    command = ['/sbin/initctl', 'stop', proc_name]
    try:
        output = subprocess.check_output(
            command,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
        )
        logger.info(output)
    except subprocess.CalledProcessError as e:
        e.args += ('Failed to stop {}: {}.'.format(proc_name, e.output),)
        raise


def _start_process(proc_name, *args):
    """Start a process.

    :param proc_name: The name of the process.
    :param args: The arguments to be used when starting the job.
    :return: The process id of the started job.
    :raises CalledProcessError: if the job failed to start.

    """
    logger.info('Starting job {} with arguments {}.'.format(proc_name, args))
    command = ['/sbin/initctl', 'start', proc_name] + list(args)
    try:
        output = subprocess.check_output(
            command,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
        )
        logger.info(output)
        pid = get_process_pid(proc_name)
    except subprocess.CalledProcessError as e:
        e.args += ('Failed to start {}: {}.'.format(proc_name, e.output),)
        raise
    else:
        return pid


def get_process_pid(proc_name):
    """Return the process id of a running job.

    :param str name: The name of the job.
    :raises ToolkitException: if the job is not running.

    """
    status = get_process_status(proc_name)
    if "start/" not in status:
        raise ToolkitException(
            '{} is not in the running state.'.format(proc_name))
    return int(status.split()[-1])


def get_process_status(name):
    """
    Return the status of a process.

    :param str name: The name of the process.
    :return: Status of process as string, or zero length string if process
             is not found.

    """
    try:
        return subprocess.check_output([
            '/sbin/initctl',
            'status',
            name
        ], universal_newlines=True, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        return ''


def is_process_running(name):
    """Return True if the process is running. Otherwise, False.

    :param str name: The name of the process.
    :raises ToolkitException: if not possible to get status of the process.

    """
    return 'start/' in get_process_status(name)


def is_maliit_process_running():
    """Return True if malitt-server process is running, False otherwise."""
    return is_process_running(MALIIT)


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
