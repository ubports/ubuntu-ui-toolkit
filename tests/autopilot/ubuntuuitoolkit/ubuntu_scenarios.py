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

from autopilot import platform


NEXUS4_DEVICE = 'Nexus4'
NEXUS10_DEVICE = 'Nexus10'
DEFAULT_DEVICES = (NEXUS4_DEVICE, NEXUS10_DEVICE)


def get_device_simulation_scenarios(devices=DEFAULT_DEVICES):
    """Return a list of devices to be simulated on tests.

    :param devices: The device or devices to simulate. Default value is all the
        officially supported devices.
    :type devices: string or sequence of strings.
    :return: A list of scenarios to be used with the testscenarios python
        module, with the values of app_width, app_height and grid_unit
        corresponding to the selected device.

    """
    if platform.model() == 'Desktop':
        return _get_device_simulation_scenarios_for_desktop(devices)
    else:
        return [(platform.model(), {})]


def _get_device_simulation_scenarios_for_desktop(devices):
    scenarios = []
    if NEXUS4_DEVICE in devices:
        scenarios.append(
            ('Simulating Nexus 4 in desktop',
             dict(app_width=768, app_height=1280, grid_unit_px=18))
        )
    if NEXUS10_DEVICE in devices:
        scenarios.append(
            ('Simulating Nexus 10 in desktop',
             dict(app_width=2560, app_height=1600, grid_unit_px=20))
        )
    return scenarios
