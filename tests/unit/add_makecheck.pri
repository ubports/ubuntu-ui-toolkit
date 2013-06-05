# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.

check.target = check
check.commands = QML2_IMPORT_PATH=../../.. UITK_THEME_PATH=../../.. ./$$TARGET -platform minimal
check.commands += -maxwarnings 10 -o ../../test_$(TARGET).xml,xunitxml -o -,txt
#check.commands += ../testparser/testparser ../../test_$(TARGET).xml 
