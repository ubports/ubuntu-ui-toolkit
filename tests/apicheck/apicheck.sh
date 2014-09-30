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
CPP="Ubuntu.Components Ubuntu.Layouts Ubuntu.Components.ListItems Ubuntu.PerformanceMetrics Ubuntu.Test"

ERRORS=0
echo Dumping QML API as JSON
for i in $CPP; do
    j=1.0
    test "$i" = "Ubuntu.Components" && j=1.1
    echo "Processing $i $j"
    # Silence spam on stderr due to fonts
    # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
    # https://bugreports.qt-project.org/browse/QTBUG-36243
    env ALARM_BACKEND=memory $(dirname $0)/apicheck --json $i $j modules 1> $i.api.new
    test $? != 0 && echo Error: apicheck failed && ERRORS=1
    echo Verifying the diff between existing and generated API
    diff -u $i.api $i.api.new
    test $? != 0 && echo Error: Differences in API. Did you forget to update $i.api? && ERRORS=1
done

if [ "x$ERRORS" != "x1" ]; then
    echo API is all fine.
    exit 0
else
    echo API test failed with errors.
    exit 1
fi
