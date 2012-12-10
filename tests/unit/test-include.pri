include (../../qtversioncheck.pri)

TEMPLATE = app
QT += testlib qml quick
CONFIG += no_keywords

include ( plugin_dependency.pri )

include (add_makecheck.pri)
