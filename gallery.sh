#!/bin/bash
#
# Copyright 2012 Canonical Ltd.
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

source export_modules_dir.sh

if [ "$1" == "--autopilot" ]
then
	examples/ubuntu-ui-toolkit-gallery/gallery -testability &
	autopilot vis
else
	examples/ubuntu-ui-toolkit-gallery/gallery $@
fi
