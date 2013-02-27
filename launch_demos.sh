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

export UITK_THEME_PATH=.

if [ "$1" == "--phone" ]
then
	GRID_UNIT_PX=18 qmlscene --fullscreen -I modules demos/PhoneComponentShowcase.qml
elif [ "$1" == "--autopilot" ]
then
	/usr/lib/*/qt5/bin/qmlscene -I modules demos/ComponentShowcase.qml -testability &
	autopilot vis
elif [ "$1" == "--autopilot-phone" ]
then
	GRID_UNIT_PX=18 qmlscene -I modules demos/PhoneComponentShowcase.qml -testability &
	autopilot vis
else
	/usr/lib/*/qt5/bin/qmlscene -I modules demos/ComponentShowcase.qml
fi
