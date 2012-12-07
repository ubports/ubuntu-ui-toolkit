include( ../../generic.pri )

TEMPLATE = app
QT += testlib qml quick
CONFIG += no_keywords

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
LIBS += $$COMPONENTS_PATH/libUbuntuComponents.so

include( add_makecheck.pri )
