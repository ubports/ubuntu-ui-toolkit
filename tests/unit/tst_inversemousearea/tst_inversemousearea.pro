include(../test-include.pri)
QT += quick-private
SOURCES += tst_inversemouseareatest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    InverseMouseAreaPropagateEvents.qml \
    InverseMouseAreaDoNotPropagateEvents.qml \
    SensingAreaError.qml \
    InverseMouseAreaInWindow.qml \
    OverlappedMouseArea.qml
