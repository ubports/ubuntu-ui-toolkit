include(../test-include.pri)
QT += quick-private gui-private
SOURCES += tst_textinputtest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    TextFieldTest.qml \
    TextAreaTest.qml \
    TextAreaInFlickable.qml
