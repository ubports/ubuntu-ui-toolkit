# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check

# Xvfb doesn't run on armhf/qemu
!contains(QMAKE_HOST.arch,armv7l) {
 check.commands = "set -e;"
 for(TEST, TESTS) {
  check.commands += cd $$_PRO_FILE_PWD_;
  check.commands += env LD_LIBRARY_PATH=$${ROOT_BUILD_DIR}/modules/Ubuntu/Components:$${ROOT_BUILD_DIR}/modules/Ubuntu/Layouts:$${ROOT_BUILD_DIR}/modules/Ubuntu/PerformanceMetrics:$${ROOT_BUILD_DIR}/modules/Ubuntu/Test
  check.commands += '$${ROOT_SOURCE_DIR}/tests/unit/runtest.sh "$$shadowed($$_PRO_FILE_PWD_)/$${TARGET}" "$${TEST}"';
 }
}
