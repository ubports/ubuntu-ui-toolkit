# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check
check.commands = "set -e;"
for(TEST, TESTS) {
  check.commands += cd $$_PRO_FILE_PWD_;
  check.commands += env QML_IMPORT_PATH=$${ROOT_BUILD_DIR}/modules
  check.commands += QML2_IMPORT_PATH=$${ROOT_BUILD_DIR}/modules
  check.commands += UBUNTU_UI_TOOLKIT_THEMES_PATH=$${ROOT_BUILD_DIR}/modules
  check.commands += '$${ROOT_SOURCE_DIR}/tests/unit/runtest.sh "$$OUT_PWD/$${TARGET}" "$${TEST}" minimal "$${ROOT_BUILD_DIR}"';
}
