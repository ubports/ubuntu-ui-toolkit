include(../test-include.pri)
DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    tst_orientation.cpp

OTHER_FILES += \
    Defaults.qml \
    ManualOrientation.qml
