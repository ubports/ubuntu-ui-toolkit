include( ../../generic.pri )

TEMPLATE = app
QT += qml quick qmltest
CONFIG += no_keywords

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
LIBS += $$COMPONENTS_PATH/libUbuntuComponents.so
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"


include( add_qmlmakecheck.pri )

