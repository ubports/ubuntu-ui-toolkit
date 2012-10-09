TEMPLATE = app
QT += testlib qml quick
CONFIG += testcase no_keywords

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
LIBS += $$COMPONENTS_PATH/libUbuntuComponents.so
