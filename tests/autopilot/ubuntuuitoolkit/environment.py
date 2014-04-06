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

import logging
import subprocess

from autopilot import logging as autopilot_logging


logger = logging.getLogger(__name__)


def is_initctl_env_var_set(variable):
    """Check True if an initctl environment variable is set.

    :param variable: The name of the variable to check.
    :return: True if the variable is set. False otherwise.

    """
    try:
        get_initctl_env_var(variable)
        return True
    except subprocess.CalledProcessError:
        return False


def get_initctl_env_var(variable):
    """Return the value of an initctl environment variable."""
    output = subprocess.check_output(
        ['/sbin/initctl', 'get-env', variable],
        stderr=subprocess.STDOUT,
        universal_newlines=True)
    return output.rstrip()


@autopilot_logging.log_action(logger.info)
def set_initctl_env_var(variable, value):
    """Set the value of an initctl environment variable."""
    subprocess.call(
        ['/sbin/initctl', 'set-env', '%s=%s' % (variable, value)],
        stderr=subprocess.STDOUT,
        universal_newlines=True)


@autopilot_logging.log_action(logger.info)
def unset_initctl_env_var(variable):
    """Remove an initctl environment variable."""
    subprocess.call(
        ['/sbin/initctl', 'unset-env', variable],
        stderr=subprocess.STDOUT,
        universal_newlines=True)
