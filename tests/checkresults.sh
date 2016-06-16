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

WARNINGS_PATTERN='message="(.*)" type="qwarn"'
ERRORS_PATTERN='<failure'

FAILURES=0
FATAL_WARNINGS=0
EXCEPTED=0
for _XML in $*; do
    _TESTNAME=$(basename $_XML | sed -r 's@(.+)\.xml@\1@' -)

    if [ ! -f $_XML ]; then
      echo "$_XML doesn't exist"
      exit 1
    fi

    EXCEPTIONS='components_benchmark \
                tst_tabbar.11.qml \
                tst_datepicker.bug1567840.SEGFAULT.12.qml \
                tst_datepicker.bug1567840.SEGFAULT.13.qml \
                tst_textinput_touch.SEGFAULT.11.qml \
                swipearea \
                qquick_image_extension \
                tst_page.11.qml \
                tst_label13.qml \
                performance \
                tst_tabs.11.qml \
                tst_focus.13.qml \
                tst_pickerpanel.11.qml \
                tst_picker.11.qml \
                tst_picker.13.qml \
                recreateview \
                touchregistry \
                mainview \
                tst_listitems_itemselector.11.qml \
                tst_tabs_with_repeater.DEPRECATED_TOOLBAR.11.qml \
                inversemousearea \
                '

    WARNINGS=$(grep -c -P "$WARNINGS_PATTERN" $_XML)
    ERRORS=$(grep -c -P "$ERRORS_PATTERN" $_XML)
    if [ $ERRORS -ne 0 ]; then
      FAILURES_FILES="${FAILURES_FILES}  ${_TESTNAME}\n"
      ((FAILURES+=$ERRORS))
    elif [ $WARNINGS -ne 0 ]; then
      if [[ $EXCEPTIONS == *$_TESTNAME* ]]; then
        EXCEPTED_FILES="${EXCEPTED_FILES}  ${_TESTNAME}\n"
        ((EXCEPTED+=$WARNINGS))
      else
        FATAL_WARNINGS_FILES="${FATAL_WARNINGS_FILES}  ${_TESTNAME}\n"
        ((FATAL_WARNINGS+=$WARNINGS))
      fi
    elif [[ $EXCEPTIONS == *$_TESTNAME* ]]; then
      WOOT_FILES="${WOOT_FILES}  ${_TESTNAME}\n"
    fi
done

TOTAL=0
if [ -n "$FAILURES_FILES" ]; then
    ((TOTAL+=$FAILURES))
    echo $FAILURES failures which MUST be fixed:
    echo -e "$FAILURES_FILES"
fi

if [ -n "$FATAL_WARNINGS_FILES" ]; then
    ((TOTAL+=FATAL_WARNINGS))
    echo $FATAL_WARNINGS fatal warnings which MUST be fixed:
    echo -e "$FATAL_WARNINGS_FILES"
fi

if [ -n "$EXCEPTED_FILES" ]; then
    echo The following tests issued $EXCEPTED expected warnings:
    echo -e "$EXCEPTED_FILES"
fi

if [ -n "$WOOT_FILES" ]; then
    echo Woot! Known problematic tests passed!
    echo Consider removing these from EXCEPTIONS in ${0#$(pwd)/}!
    echo -e "$WOOT_FILES"
fi

exit $TOTAL
