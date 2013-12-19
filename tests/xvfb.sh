#!/bin/sh
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

echo Running $@ in virtual frame buffer...
xvfb-run -a -s "-screen 0 1280x1024x24" -e xvfb.err "$@" 2>test.err
RETVAL=$?
if [ $RETVAL -eq 0 ]; then
    echo $@ finished successfully...
else
    echo $@ in virtual frame buffer failed...
    cat test.err >&2
    echo Tail of xvfb-run output:
    tail xvfb.err >&2
    exit $RETVAL
fi
