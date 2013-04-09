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
_ARGS="-platform minimal -xunitxml -o ../../test_$_TARGET_$_TESTFILE.xml"
set +e

function create_test_cmd {
  _CMD="./$_TARGET -input $_TESTFILE -import \"../../../modules\" -maxwarnings 20"
}

function execute_test_cmd {
  echo "Executing $_CMD $_ARGS"
  UITK_THEME_PATH=../../.. $_CMD $_ARGS
  # segfault
  if [ $? -eq 139 ]; then
   return 2
  fi
  # abort
  if [ $? -eq 134 ]; then
   return 2
  fi
  ../testparser/testparser ../../test_$_TARGET_$_TESTFILE.xml;
  return 0
}

create_test_cmd
execute_test_cmd
if [ $? -eq 2 ]; then
  echo "FAILURE: Failed to execute test with -platform minimal, lets try without"
  _ARGS=""
  execute_test_cmd
  if [ $? -eq 2 ]; then
   echo "FAILURE: Failed to execute test."
   set -e
   exit -2
  fi
fi

set -e
