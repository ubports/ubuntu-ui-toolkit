include(../test-include.pri)
QT += quick-private
SOURCES += tst_mousefilterstest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    FilterTextInput.qml \
    PressAndHoldAcceptedEatsClick.qml \
    PressAndHoldAndClicked.qml \
    FilterInverseTextInput.qml
