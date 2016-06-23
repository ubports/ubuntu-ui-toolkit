include( ../unit/plugin_dependency.pri )
include( layout_dependency.pri )
include( add_makecheck_x11.pri )

TEMPLATE = app
QT += testlib qml quick UbuntuToolkit UbuntuToolkit_private
CONFIG += no_keywords c++11
