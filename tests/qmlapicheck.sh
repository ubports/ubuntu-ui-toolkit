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
################################################################################

source `dirname $0`/../export_qml_dir.sh || exit 1

if [ ! -e $BUILD_DIR/qml/Ubuntu/Layouts/libUbuntuLayouts.so ]; then
    echo You need to build UITK before you can dump QML API!
    exit 1
fi

CPP="Ubuntu.Components Ubuntu.Components.ListItems Ubuntu.Components.Popups Ubuntu.Components.Pickers Ubuntu.Components.Styles Ubuntu.Components.Themes Ubuntu.Layouts Ubuntu.PerformanceMetrics Ubuntu.Metrics Ubuntu.Test"
echo Dumping QML API of C++ components
test -s $BUILD_DIR/components.api.new && rm $BUILD_DIR/components.api.new
env ALARM_BACKEND=memory \
    $BUILD_DIR/apicheck/apicheck \
    --qml $CPP 1>> $BUILD_DIR/components.api.new &&
    echo Verifying the diff between existing and generated API
if [ $? -gt 0 ]; then
    echo Error: apicheck failed
else
    diff -F '[.0-9]' -u $SRC_DIR/components.api $BUILD_DIR/components.api.new && \
        echo API is all fine. && exit 0
    echo Differences in API. Did you forget to update components.api?
    exit 1
fi
