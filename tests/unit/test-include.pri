include (../../qtversioncheck.pri)
TEMPLATE = app
QT += testlib qml quick
CONFIG += no_keywords

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
LIBS += $$COMPONENTS_PATH/libUbuntuComponents.so

# Do not use CONFIG += testcase that would add a 'make check' because it also
# adds a 'make install' that installs the test cases, which we do not want.
# Instead add a 'make check' manually.
check.target = check
check.commands = ./$$TARGET -platform minimal -xunitxml >
check.commands += ../../test_$(TARGET).xml
