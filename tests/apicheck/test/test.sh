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

. `dirname $0`/../../../build_paths.inc

if [ ! -e $BUILD_DIR/tests/apicheck/apicheck ]; then
    echo API check tool not built!
    exit 1
fi

echo Self-testing API dump
# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
env QML2_IMPORT_PATH=$SRC_DIR/tests/apicheck/test LANG=C \
    $BUILD_DIR/tests/apicheck/apicheck --qml Extinct.Animals | \
    diff -F '[.0-9]' -u $SRC_DIR/tests/apicheck/test/components.api -
test $? != 0 && echo Error: Mismatch || echo All fine.
