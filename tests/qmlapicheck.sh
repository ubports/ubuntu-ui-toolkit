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
QML="modules/Ubuntu/*/qmldir modules/Ubuntu/Components/Colors/UbuntuColors.qml modules/Ubuntu/Components/*/qmldir"
CPP="Ubuntu.Components Ubuntu.Layouts"

echo Dumping QML API of C++ components
echo '' > plugins.qmltypes
for i in $CPP; do
    qmlplugindump $i 0.1 modules >> plugins.qmltypes
done
STATUS=$?
test $STATUS = 0 || ERRORS=1
test $STATUS = 0 || echo $RESULTS

echo Running QML API check for $QML
# Palette gets included in Qt 5.2 qmlplugindump even though it's qml
BUILTINS=QQuick,QQml,U1db::,Palette python tests/qmlapicheck.py $QML plugins.qmltypes > components.api.new
STATUS=$?
test $STATUS = 0 || ERRORS=1
test $STATUS = 0 || echo $RESULTS

echo Verifying the diff between existing and generated API
diff -Fqml -u components.api components.api.new
STATUS=$?
test $STATUS = 0 || ERRORS=1
test $STATUS = 0 || echo $RESULTS

if [ "x$ERRORS" != "x1" ]; then
    echo API is all fine.
    exit 0
else
    echo API test failed with errors. Did you forget to update components.api?
    exit 1
fi
