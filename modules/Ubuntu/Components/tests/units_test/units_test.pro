TEMPLATE = app
QT += testlib qml quick
CONFIG += testcase no_keywords
SOURCES += units_test.cpp
INCLUDEPATH += ../../plugin
PRE_TARGETDEPS = ../../libUbuntuComponents.so
LIBS += ../../libUbuntuComponents.so
