include(../test-include.pri)
SOURCES += tst_inversemouseareatest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    InverseMouseAreaPropagateEvents.qml \
    InverseMouseAreaDoNotPropagateEvents.qml
