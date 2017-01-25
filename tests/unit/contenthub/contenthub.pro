include(../test-include-x11.pri)
QT += dbus gui
SOURCES += tst_contenthub.cpp
OTHER_FILES += TextAreaPaste.qml TextFieldPaste.qml
DBUS_ADAPTORS += com.ubuntu.content.MockService.xml
