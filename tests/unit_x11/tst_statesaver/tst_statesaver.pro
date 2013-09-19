include(../test-include.pri)
DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    tst_statesaver.cpp

OTHER_FILES += \
    SaveBool.qml \
    SaveInt.qml \
    SaveReal.qml \
    SaveString.qml
