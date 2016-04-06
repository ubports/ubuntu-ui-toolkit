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

source $PWD/export_qml_dir.sh

if [ ! -e $QML2_IMPORT_PATH/Ubuntu/Layouts/libUbuntuLayouts.so ]; then
    echo You need to build UITK before you can run Autopilot test cases!
    exit 1
fi

MODES='run|list|vis|launch'
usage(){
    echo Usage:
    echo "  " $(basename $0) "[-h] [$MODES] SUITE"
    exit 1
}

cd tests/autopilot

export UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE=1
export UITK_BUILD_ROOT="$BUILD_DIR"
export UITK_SOURCE_ROOT="$SRC_DIR"
MODE="run"
SUITE=ubuntuuitoolkit

if [ "$1" == "" ]; then
    usage
elif [[ $MODES == *$1* ]]; then
    MODE=$1
    if [ "$2" == "" ]; then
        set "$SUITE"
    else
        shift 1
    fi
elif [ "$2" = "" ]; then
    true
elif [[ $MODES == *$2* ]]; then
    MODE=$2
    if [ "$3" == "" ]; then
        shift 1
        set "$SUITE"
    else
        shift 2
    fi
fi

if [ "$MODE" = "run" ]; then
    autopilot3 $MODE -o ../../$SUITE -f xml -r -rd ../../ $*
else
    autopilot3 $MODE $*
fi

RESULT=$?
if [ "$RESULT" == "2" ]; then
    usage
fi
exit $RESULT
