include( ../unit/plugin_dependency.pri )
include( add_qmlmakecheck.pri)

TEMPLATE = app
QT += qml quick qmltest
CONFIG += no_keywords

QMAKE_CXXFLAGS += -Werror
