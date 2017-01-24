include(../test-include.pri)
QT += dbus gui
SOURCES += tst_contenthub.cpp
DBUS_ADAPTORS += com.ubuntu.content.MockService.xml
