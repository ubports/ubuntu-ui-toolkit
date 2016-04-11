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

set -e

source `dirname $0`/../../export_qml_dir.sh

_CMD=""
_TARGETPATH=$1
_TESTFILEPATH=$2
_MINIMAL=$3

relpath() {
  python -c 'import os, sys; print(os.path.relpath(*sys.argv[1:]))' $*
}

if [ -z $_TESTFILEPATH ]; then
  _TESTFILEPATH=$_TARGETPATH
elif [[ 'minimal custom' == *$_TESTFILEPATH* ]]; then
  _MINIMAL=$_TESTFILEPATH
  _TESTFILEPATH=$_TARGETPATH
fi
test -z $_MINIMAL && _MINIMAL=default

if [ -z $_TARGETPATH ]; then
  echo Usage:
  echo "  $0 TEST_EXECUTABLE [QML_FILE] [QT_QPA_PLATFORM]"
  echo ''
  echo 'Examples:'
  echo "  $0 $(relpath ${BUILD_DIR}/tests/unit/tst_components/tst_components) tst_label13.qml minimal"
  echo ''
  echo "  cd $(relpath ${BUILD_DIR}/tests/unit/tst_mainview)"
  echo "  ../$(basename $0) tst_mainview minimal"
  echo "  cd ../../.."
  echo ''
  echo "  cd $(relpath ${BUILD_DIR}/tests/unit_x11/tst_components)"
  echo "  ../../xvfb.sh ../../unit/$(basename $0) tst_components tst_listitem13.qml"
  echo "  cd ../../.."
  echo ''
  echo "  $(relpath ${BUILD_DIR}/tests/xvfb.sh) $0 $(relpath ${BUILD_DIR}/tests/unit_x11/tst_bottomedge/tst_bottomedge)"
  exit 1
fi

_TARGET=$(basename $_TARGETPATH)
_TESTFILE=$(basename $_TESTFILEPATH)
_XML="${BUILD_DIR}/tests/test_$_TARGET_$_TESTFILE.xml"

_ARGS="-p -o -p $_XML,xunitxml -p -o -p -,txt"

function create_fallback_xml {
  cat << EOF > $_XML
<?xml version="1.0" encoding="UTF-8" ?>
<testsuite errors="1" failures="1" tests="1" name="$_TESTFILE">
  <testcase result="fail" name="${_TESTFILE}_Execution">
    <failure message="$1" result="fail"/>
  </testcase>
</testsuite>
EOF
}

function create_test_cmd {
  if [[ "$_TARGETPATH" = /* ]]; then
      EXE=$_TARGETPATH
  else
      EXE=./$_TARGETPATH
  fi
  _CMD="-n $_TESTFILE -m 500"

  DEB_HOST_ARCH=$(dpkg-architecture -qDEB_HOST_ARCH)
  if [[ ${DEB_HOST_ARCH} =~ 'arm' ]]; then
    _CMD="dbus-test-runner --task $(readlink -f $EXE) $_CMD"
  else
    _CMD="dbus-test-runner --task gdb -p --quiet $_CMD"
    _CMD="$_CMD -p --batch -p -ex -p 'set print thread-events off' -p -ex -p run -p -ex -p bt -p --return-child-result -p --args -p $(readlink -f $EXE)"
  fi

  if [[ 'minimal custom' == *$_MINIMAL* ]]; then
      _CMD="$_CMD -p -platform -p $_MINIMAL"
  fi

  if [[ $_TESTFILEPATH == *\.qml* ]]; then
      _CMD="$_CMD -p -input -p $_TESTFILEPATH"
  fi
  _CMD="$_CMD -p -maxwarnings -p 100"
}

function execute_test_cmd {
  echo "Executing $_CMD $_ARGS"
  echo "Working directory: $PWD"
  if [ ! -x $_TARGETPATH ]; then
    echo "Error: $_TARGET wasn't built!"
    RESULT=2
  elif [ $DISPLAY ]; then
    cd $(dirname $_TARGETPATH)

    # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
    # https://bugreports.qt-project.org/browse/QTBUG-36243
	
    ALARM_BACKEND=memory SUPPRESS_DEPRECATED_NOTE=no \
    QT_LOGGING_RULES="[PERFORMANCE].warning=false" \
    $_CMD $_ARGS 2>&1 | grep -v 'QFontDatabase: Cannot find font directory'
    if [ ! -s $_XML ]; then
        # Write fallback in case it crashed and the file is empty
        create_fallback_xml "Test results corrupted (crash)"
    fi
    if [ "x$UITK_TEST_KEEP_RUNNING" != "x1" ]; then
        ${BUILD_DIR}/tests/checkresults.sh $_XML
        RESULT=$?
    fi
  else
    echo "Skipped because no DISPLAY available"
  fi
  if [ -z $RESULT ]; then
    RESULT=0
  # segfault
  elif [ $RESULT -eq 139 ]; then
    RESULT=2
  # abort
  elif [ $RESULT -eq 134 ]; then
    RESULT=2
  fi
  echo "$_TARGET_$_TESTFILE exited with $RESULT"
  return $RESULT
}

# Always create XML in case the test can't be run, eg. .so file missing
create_fallback_xml "Test couldn't be run"
create_test_cmd
execute_test_cmd
RESULT=$?
exit $RESULT
