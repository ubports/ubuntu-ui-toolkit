#!/usr/bin/env sh
#
# Copyright 2015 Canonical Ltd.
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

. `dirname $0`/../../build_paths.inc

if [ ! -e $BUILD_DIR/apicheck/apicheck ]; then
    echo API check tool not built!
    exit 1
elif [ ! -e $BUILD_DIR/qml/Extinct/Animals/libExtinctAnimals.so ]; then
    echo You need to build Extinct.Animals before you can dump QML API!
    exit 1
fi

echo Self-testing API dump
env QML2_IMPORT_PATH=$BUILD_DIR/qml \
    $BUILD_DIR/apicheck/apicheck --qml Extinct.Animals | \
    diff -F '[.0-9]' -u $SRC_DIR/tests/api/components.api -
test $? != 0 && echo Error: Mismatch && exit 1
echo All fine.
