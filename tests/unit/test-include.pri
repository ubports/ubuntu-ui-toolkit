include( plugin_dependency.pri )
include( add_makecheck.pri )

TEMPLATE = app
QT += testlib qml quick systeminfo UbuntuToolkit-private
CONFIG += no_keywords c++11
