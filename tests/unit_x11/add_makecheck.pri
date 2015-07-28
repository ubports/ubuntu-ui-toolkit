# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

# Xvfb doesn't run on armhf/qemu
!contains(QMAKE_HOST.arch,armv7l) {
    check.target = check
    check.commands += cd $$_PRO_FILE_PWD_;
    check.commands += env LD_LIBRARY_PATH=$${ROOT_BUILD_DIR}/qml/Ubuntu/Components:$${ROOT_BUILD_DIR}/qml/Ubuntu/Layouts:$${ROOT_BUILD_DIR}/qml/Ubuntu/PerformanceMetrics:$${ROOT_BUILD_DIR}/qml/Ubuntu/Test UITK_TEST_KEEP_RUNNING=1
    check.commands += '$${ROOT_SOURCE_DIR}/tests/unit/runtest.sh "$$shadowed($$_PRO_FILE_PWD_)/$${TARGET}" "$$shadowed($$_PRO_FILE_PWD_)/$${TARGET}"';
}
