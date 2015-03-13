#!/usr/bin/env sh
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
################################################################################
PATTERN='\.(c(c|pp|xx)?|h(h|pp|xx)?|p(l|m)|php|py(|x)|java|js|css|vala|qml)$'
SKIP='(Canonical|GENERATED FILE|Yahoo! Inc. All rights reserved)'
COMMAND="licensecheck --noconf -r * --copyright -c $PATTERN"
echo Executing $COMMAND
RESULTS=$($COMMAND)
RESULT=$?
ERRORS=$(echo "$RESULTS" | egrep -v "$SKIP" | grep '*No copyright*')
COUNT=$(echo "$ERRORS" | sed 's/^ *//g' | wc -l)
if [ "$ERRORS" != "" ]; then
    echo Found $COUNT license problems:
    echo "$ERRORS"
    return $RESULT
elif [ "$RESULT" != 0 ]; then
    echo License check with unknown cause:
    echo "RESULTS"
    return $RESULT
elif [ "$ERRORS" = "" ]; then
    echo No license problems found.
    exit 0
fi
