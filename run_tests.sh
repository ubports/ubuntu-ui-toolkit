#!/bin/bash
#
# Copyright 2012 - 2015 Canonical Ltd.
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

if [ ! -e $QML2_IMPORT_PATH/Ubuntu/Layouts/libUbuntuLayouts.so ]; then
    echo You need to build UITK before you can run Autopilot test cases!
    exit 1
fi

cd tests/autopilot

export UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE=1
export UITK_BUILD_ROOT="$BUILD_DIR"
export UITK_SOURCE_ROOT="$SRC_DIR"
MODE="run"
SUITE=ubuntuuitoolkit
if [ "$1" != "" ]; then
    MODE="$1"
    shift 1
fi
if [ "$1" != "" ]; then
    SUITE="$1"
    shift 1
fi
autopilot3 $MODE -o ../../$SUITE -f xml -r -rd ../../ $SUITE $*
RESULT=$?
if [ "$RESULT" == "2" ]; then
    echo Usage:
    echo "  " $(basename $0) "run|list|vis|launch suite [-v]"
    echo "See also 'autopilot -h'"
fi
exit $RESULT
