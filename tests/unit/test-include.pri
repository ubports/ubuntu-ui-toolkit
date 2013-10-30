include( plugin_dependency.pri )
include( layout_dependency.pri )
include( add_makecheck.pri )

TEMPLATE = app
QT += testlib qml quick
CONFIG += no_keywords

QMAKE_CXXFLAGS += -Werror

