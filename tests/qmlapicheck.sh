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

QML="modules/Ubuntu/*/qmldir modules/Ubuntu/Components/*/qmldir"
CPP="Ubuntu.Components Ubuntu.Layouts Ubuntu.PerformanceMetrics Ubuntu.Test"

echo Dumping QML API of C++ components
echo '' > $BUILD_DIR/plugins.qmltypes
ERRORS=0
for i in $CPP; do
    # Silence spam on stderr due to fonts
    # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
    # https://bugreports.qt-project.org/browse/QTBUG-36243
    env UBUNTU_UI_TOOLKIT_THEMES_PATH=$BUILD_DIR/modules ALARM_BACKEND=memory \
        qmlplugindump -noinstantiate $i 0.1 $BUILD_DIR/modules 1>> $BUILD_DIR/plugins.qmltypes
    test $? != 0 && ERRORS=1
done
test $ERRORS = 1 && echo Error: qmlplugindump failed && exit 1

echo Running QML API check for $QML
# Palette and UbuntuColors gets included in Qt > 5.2 qmlplugindump even though it's qml
BUILTINS=QQuick,QQml,U1db::,Palette,UbuntuColors python3 tests/qmlapicheck.py $QML $BUILD_DIR/plugins.qmltypes > $BUILD_DIR/components.api.new
test $? != 0 && echo Error: qmlapicheck.py failed && exit 1

echo Verifying the diff between existing and generated API
diff -Fqml -u components.api $BUILD_DIR/components.api.new
test $? != 0 && ERRORS=1

if [ "x$ERRORS" != "x1" ]; then
    echo API is all fine.
    exit 0
else
    echo API test failed with errors. Did you forget to update components.api?
    exit 1
fi
