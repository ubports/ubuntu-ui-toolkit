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
_ARGS="-o ../../test_$_TARGET_$_TESTFILE.xml,xunitxml -o -,txt"
set +e

function create_test_cmd {
  _CMD="./$_TARGET"
  if [ "$_MINIMAL" = "minimal" ]; then
      _CMD="$_CMD -platform minimal"
  fi
  if [ $_TARGET != $_TESTFILE ]; then
      _CMD="$_CMD -input $_TESTFILE"
  fi
  _CMD="$_CMD -maxwarnings 20"
}

function execute_test_cmd {
  echo "Executing $_CMD $_ARGS"
  if [ $DISPLAY ]; then
      # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
      QML2_IMPORT_PATH=../../../modules:$QML2_IMPORT_PATH UBUNTU_UI_TOOLKIT_THEMES_PATH=../../../modules \
          $_CMD $_ARGS 2>&1 | grep -v 'QFontDatabase: Cannot find font directory'
  else
      echo "Skipped because no DISPLAY available"
  fi
  RESULT=$?
  # segfault
  if [ $RESULT -eq 139 ]; then
   return 2
  fi
  # abort
  if [ $RESULT -eq 134 ]; then
   return 2
  fi
  return $RESULT
}

create_test_cmd
execute_test_cmd
RESULT=$?
exit $RESULT
