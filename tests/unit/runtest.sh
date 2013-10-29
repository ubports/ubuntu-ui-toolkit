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
_ARG_XML="-o ../../test_$_TARGET_$_TESTFILE.xml,xunitxml -o -,txt"
_ARGS=""
set +e

function create_test_cmd {
  _CMD="./$_TARGET -input $_TESTFILE -import \"../../../modules\" -maxwarnings 20"
}

function execute_test_cmd {
  echo "Executing $_CMD $_ARGS"
  QML2_IMPORT_PATH=../../../modules:$QML2_IMPORT_PATH UBUNTU_UI_TOOLKIT_THEMES_PATH=../../../modules $_CMD $_ARGS
  RESULT=$?
  # segfault
  if [ $RESULT -eq 139 ]; then
   return 2
  fi
  # abort
  if [ $RESULT -eq 134 ]; then
   return 2
  fi
#  if [ $RESULT -eq 0 ]; then
#    ../testparser/testparser ../../test_$_TARGET_$_TESTFILE.xml;
#  fi
  return $RESULT
}

create_test_cmd
execute_test_cmd
RESULT=$?
exit $RESULT
