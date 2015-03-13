#!/bin/bash
#
# Copyright 2014-2015 Canonical Ltd.
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
################################################################################

. `dirname $0`/../build_paths.inc

FAILURES=0
for i in $*; do
    _XML=$i
    _TESTNAME=$(basename $_XML | sed -r 's@test_(.+)\.xml@\1@' -)

    if [ ! -f $_XML ]; then
      echo "$_XML doesn't exist"
      exit 1
    fi

    EXCEPTIONS='tst_components_benchmark \
                tst_tabbar.qml \
                tst_datepicker.qml \
                tst_qquick_image_extension \
                tst_page.qml \
                tst_toolbar.qml \
                tst_tabs.qml \
                tst_focus.qml \
                tst_pickerpanel.qml \
                tst_picker.qml \
                tst_listitems_itemselector.qml \
                tst_tabs_with_repeater.deprecated_toolbar.qml \
                '

    WARNINGS=$(grep -c -e 'qwarn' $_XML)
    ERRORS=$(grep -c -e 'result="fail"' $_XML)
    if [ $WARNINGS -ne 0 ]; then
      if [[ $EXCEPTIONS == *$_TESTNAME* ]]; then
        echo "FIXME: $WARNINGS warnings in $_TESTNAME - Known problematic test"
      else
        echo "Error: $WARNINGS warnings in $_TESTNAME"
        FAILURES=$(echo "$FAILURES+1" | bc)
      fi
    elif [ $ERRORS -ne 0 ]; then
      echo "Error: $ERRORS errors in $_TESTNAME ($WARNINGS warnings)"
      FAILURES=$(echo "$FAILURES+1" | bc)
    elif [[ $EXCEPTIONS == *$_TESTNAME* ]]; then
      echo Woot! Known problematic test $_TESTNAME did pass afterall!
      echo '  ' Consider removing $_TESTNAME from EXCEPTIONS in $0
    fi
done

if [ $FAILURES -ne 0 ]; then
    exit 666
fi
