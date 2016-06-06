# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check

# Xvfb doesn't run on armhf/qemu
!contains(QMAKE_HOST.arch,armv7l) {
 check.commands = "set -e;"
 for(TEST, TESTS) {
  _uitk_command = cd $$_PRO_FILE_PWD_;
  _uitk_command += env UITK_TEST_KEEP_RUNNING=1
  _uitk_command += '$${ROOT_SOURCE_DIR}/tests/unit/runtest.sh "$$shadowed($$_PRO_FILE_PWD_)/$${TARGET}" "$${_PRO_FILE_PWD_}/$${TEST}"';

  check.commands += $${_uitk_command}

  #add a convenience target per TEST file
  check_name = "check_$${TEST}"
  check_name = $$replace(check_name, "\.qml", "")
  check_name = $$replace(check_name, "\.", "_")
  $${check_name}.target   = $${check_name}
  $${check_name}.commands += $${_uitk_command}
  QMAKE_EXTRA_TARGETS+=$${check_name}
 }
}
