#!/bin/bash
#
# Copyright 2012 Canonical Ltd.
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
# Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
################################################################################

_CMD=""
_TARGET=$1
_TESTFILE=$2
_MINIMAL=$3
_XML="../../test_$_TARGET_$_TESTFILE.xml"
_ARGS="-o $_XML,xunitxml -o -,txt"
set +e

function create_test_cmd {
  _CMD="./$_TARGET"
  if [ "$_MINIMAL" = "minimal" ]; then
      _CMD="$_CMD -platform minimal"
  fi
  if [ $_TARGET != $_TESTFILE ]; then
      _CMD="$_CMD -input $_TESTFILE"
  fi
  _CMD="$_CMD -maxwarnings 40"
}

function execute_test_cmd {
  echo "Executing $_CMD $_ARGS"
  if [ ! -x $_TARGET ]; then
    echo "Error: $_TARGET wasn't built!"
    RESULT=2
  elif [ $DISPLAY ]; then
    # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
    # https://bugreports.qt-project.org/browse/QTBUG-36243
    QML2_IMPORT_PATH=../../../modules:$QML2_IMPORT_PATH UBUNTU_UI_TOOLKIT_THEMES_PATH=../../../modules \
    ALARM_BACKEND=memory \
    $_CMD $_ARGS 2>&1 | grep -v 'QFontDatabase: Cannot find font directory'
    # Note: Get first command before the pipe, $? would be ambiguous
    RESULT=${PIPESTATUS[0]}
    WARNINGS=$(grep -c qwarn $_XML)
    EXCEPTIONS='tst_components_benchmark \
                tst_tabbar.qml \
                tst_datepicker.qml \
                tst_qquick_image_extension \
                tst_page.qml \
                tst_toolbar.qml \
                tst_tabs.qml \
                '
    if [ $WARNINGS -ne 0 ]; then
      if [[ $EXCEPTIONS == *$_TARGET_$_TESTFILE* ]]; then
        echo "FIXME: $WARNINGS warnings - Known problematic test"
      else
        echo "Error: $WARNINGS warnings in $_TARGET_$_TESTFILE"
        RESULT=666
      fi
    elif [[ $EXCEPTIONS == *$_TARGET_$_TESTFILE* ]]; then
      echo Woot! Known problematic test did pass afterall!
      echo Consider removing $_TARGET_$_TESTFILE from EXCEPTIONS in $0
    fi
  else
    echo "Skipped because no DISPLAY available"
    RESULT=0
  fi
  # segfault
  if [ $RESULT -eq 139 ]; then
    RESULT=2
  fi
  # abort
  if [ $RESULT -eq 134 ]; then
    RESULT=2
  fi
  echo "$_TARGET_$_TESTFILE exited with $RESULT"
  return $RESULT
}

create_test_cmd
execute_test_cmd
RESULT=$?
exit $RESULT
