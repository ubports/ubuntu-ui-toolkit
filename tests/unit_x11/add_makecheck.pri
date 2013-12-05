# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

# coverage is run outside of xvfb.sh
!CONFIG(coverage){
# Xvfb doesn't run on armhf/qemu
!contains(QMAKE_HOST.arch,armv7l) {
    check.target = check
    check.commands = QML2_IMPORT_PATH=../../.. UBUNTU_UI_TOOLKIT_THEMES_PATH=../../../modules ./$$TARGET
    check.commands += -maxwarnings 10 -o ../../test_$(TARGET).xml,xunitxml -o -,txt
}
}
