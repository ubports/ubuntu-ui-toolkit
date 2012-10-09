TEMPLATE = app
QT += testlib qml quick
CONFIG += testcase no_keywords
SOURCES += scaling_image_provider_test.cpp
INCLUDEPATH += ../../plugin
PRE_TARGETDEPS = ../../libUbuntuComponents.so
LIBS += ../../libUbuntuComponents.so
