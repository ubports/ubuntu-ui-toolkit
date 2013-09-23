include(../test-include.pri)
DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    tst_statesaver.cpp

OTHER_FILES += \
    SaveArrays.qml \
    SaveSupportedTypes.qml \
    ValidUID.qml \
    InvalidUID.qml \
    ValidGroupProperty.qml \
    InvalidGroupProperty.qml \
    Dynamic.qml \
    TwoDynamics.qml \
    DisabledStateSaver.qml \
    SaveObject.qml \
    FirstComponent.qml \
    SecondComponent.qml \
    SameIdsInDifferentComponents.qml
