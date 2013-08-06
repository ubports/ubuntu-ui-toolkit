# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

DISPLAY = $$system(export|grep DISPLAY)

!isEmpty(DISPLAY) {
    check.target = check
    check.commands = QML2_IMPORT_PATH=../../.. UBUNTU_UI_TOOLKIT_THEMES_PATH=../../../modules ./$$TARGET
    check.commands += -maxwarnings 10 -o ../../test_$(TARGET).xml,xunitxml -o -,txt
    #check.commands += ../testparser/testparser ../../test_$(TARGET).xml
}

isEmpty(DISPLAY) {
  message("DISPLAY is not set. Skip tests.");
}
