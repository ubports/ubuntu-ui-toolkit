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

# xvfb-run can't load gl drivers, Xvfb can
NUM=0
while [ -f /tmp/.X$NUM-lock ]; do
    NUM=$(($NUM + 1))
done
DISPLAY=":$NUM"
trap : USR1
(trap '' USR1; exec Xvfb $DISPLAY -screen 0 1280x1024x24 2>xvfb.err) &
EPID=$!
echo Running $* in virtual frame buffer with $EPID...
wait $EPID || :
if kill -0 $EPID; then
    echo Executing test...
    env DISPLAY=$DISPLAY $*
    RETVAL=$?
    echo Terminating...
    kill -9 $EPID
    echo Finished returning $RETVAL...
    exit $RETVAL
else
    echo Xvfb failed to start...
    cat xvfb.err
    exit 1
fi
