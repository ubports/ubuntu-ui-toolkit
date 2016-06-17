#!/bin/bash
#
# Copyright 2016 Canonical Ltd.
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

SCRIPT_DIRECTORY=`dirname $0`
cd $SCRIPT_DIRECTORY

TEST_FILES=$(ls tst_*.??.qml)
QML_FILES=$(ls tst_*.qml)
[ "$TEST_FILES" == "$QML_FILES" ] && exit 0
echo "Some unit tests don't have a proper version suffix:"
for FILENAME in $QML_FILES; do
    if [[ $TEST_FILES != *$FILENAME* ]]; then
        echo "  $FILENAME"
    fi
done
exit 1
