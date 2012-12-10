include (../../qtversioncheck.pri)

TEMPLATE = app
QT += qml quick qmltest
CONFIG += no_keywords

include ( plugin_dependency.pri )
include (add_qmlmakecheck.pri)

