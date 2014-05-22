#!/bin/bash
#
# Copyright 2012, 2013 Canonical Ltd.
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
#

source $PWD/export_modules_dir.sh

cd tests/autopilot

export UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE=1
SUITE=ubuntuuitoolkit
if [ "$1" != "" ]; then
    SUITE="$1"
    shift 1
fi
autopilot3 run -o ../../$SUITE -f xml -r -rd ../../ $SUITE $*

exit 0

