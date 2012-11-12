include (../../qtversioncheck.pri)

TEMPLATE = app
QT += qml quick qmltest
CONFIG += no_keywords

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
LIBS += $$COMPONENTS_PATH/libUbuntuComponents.so
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"

check.target = check
check.commands = ./$$TARGET -platform minimal
check.commands += -import \"../../../modules\" -xml >
check.commands += ../../test_$(TARGET).xml

