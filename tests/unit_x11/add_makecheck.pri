# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

# Xvfb doesn't run on armhf/qemu
!contains(QMAKE_HOST.arch,armv7l) {
    check.target = check
    check.commands += env QML_IMPORT_PATH=$${ROOT_BUILD_DIR}/modules
    check.commands += QML2_IMPORT_PATH=$${ROOT_BUILD_DIR}/modules
    check.commands += UBUNTU_UI_TOOLKIT_THEMES_PATH=$${ROOT_BUILD_DIR}/modules
    check.commands += '$${ROOT_SOURCE_DIR}/tests/unit/runtest.sh "$$OUT_PWD/$${TARGET}" "$$OUT_PWD/$${TARGET}" "" "$${ROOT_BUILD_DIR}"';
}
