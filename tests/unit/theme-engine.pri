include( ../../qtversioncheck.pri )

INCLUDEPATH += ../../../modules/Ubuntu/Components/plugin

PRE_TARGETDEPS += ../../../modules/Ubuntu/Components/libUbuntuComponents.so
LIBS += ../../../modules/Ubuntu/Components/libUbuntuComponents.so

OTHER_FILES += \
    ../../resources/test.qthm \
    ../../resources/base.qthm

RESOURCES += \
    ../../resources/theme-engine-test.qrc
