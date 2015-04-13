# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2015 Canonical Ltd.
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

"""Ubuntu UI Toolkit units helpers."""

import os

from ubuntuuitoolkit import environment


ENV_GRID_UNIT_PX = 'GRID_UNIT_PX'
DEFAULT_GRID_UNIT_PX = 8


def get_grid_unit():
    grid_unit_px = os.environ.get(ENV_GRID_UNIT_PX, None)
    if not grid_unit_px and environment.is_initctl_env_var_set(
            ENV_GRID_UNIT_PX):
        grid_unit_px = environment.get_initctl_env_var(ENV_GRID_UNIT_PX)
    return float(grid_unit_px or DEFAULT_GRID_UNIT_PX)


def gu(value):
    """"Return the number of pixels corresponding to value in grid units."""
    return value * get_grid_unit()
