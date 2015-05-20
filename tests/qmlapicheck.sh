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

. `dirname $0`/../build_paths.inc

if [ ! -e $BUILD_DIR/modules/Ubuntu/Layouts/libUbuntuLayouts.so ]; then
    echo You need to build UITK before you can dump QML API!
    exit 1
fi

QML="modules/Ubuntu/*/qmldir modules/Ubuntu/Components/*/qmldir"
CPP="Ubuntu.Components Ubuntu.Components.ListItems Ubuntu.Components.Popups Ubuntu.Components.Styles Ubuntu.Components.Themes Ubuntu.Layouts Ubuntu.PerformanceMetrics Ubuntu.Test"

ERRORS=0
echo Dumping QML API of C++ components
test -s $BUILD_DIR/components.api.new && rm $BUILD_DIR/components.api.new
    # Silence spam on stderr due to fonts
    # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
    # https://bugreports.qt-project.org/browse/QTBUG-36243
    env ALARM_BACKEND=memory QML2_IMPORT_PATH=$BUILD_DIR/modules \
        $BUILD_DIR/tests/apicheck/apicheck \
        --qml $CPP 1>> $BUILD_DIR/components.api.new &&
        echo Verifying the diff between existing and generated API
    test $? != 0 && echo Error: apicheck failed && ERRORS=1
if [ "x$ERRORS" != "x1" ]; then
    diff -F '[.0-9]' -u $SRC_DIR/components.api $BUILD_DIR/components.api.new
    test $? != 0 && echo Error: Differences in API. Did you forget to update components.api? && ERRORS=1
fi

if [ "x$ERRORS" != "x1" ]; then
    echo API is all fine.
    exit 0
else
    echo API test failed with errors.
    exit 1
fi
