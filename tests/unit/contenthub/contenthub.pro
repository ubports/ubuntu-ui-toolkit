include(../test-include.pri)
QT += dbus
SOURCES += tst_contenthub.cpp
DBUS_ADAPTORS += com.ubuntu.content.MockService.xml
