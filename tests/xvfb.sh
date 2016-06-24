#!/bin/bash
#
# Copyright 2013 Canonical Ltd.
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
# Author: Christian Dywan <christian.dywan@canonical.com>

. `dirname ${BASH_SOURCE[0]}`/../export_qml_dir.sh || exit 1

LOG=$BUILD_DIR/xvfb.err
echo Running $@ in virtual frame buffer...
xvfb-run -a -s "-screen 0 1280x1024x24" -e $LOG "$@" 2>$LOG
RETVAL=$?
if [ $RETVAL -eq 0 ]; then
    echo $@ finished successfully...
else
    echo $@ in virtual frame buffer failed...
    cat $LOG >&2
    echo Tail of xvfb-run output:
    tail $LOG >&2
    exit $RETVAL
fi
