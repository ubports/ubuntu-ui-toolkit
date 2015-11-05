include(../test-include.pri)
QT += core-private qml-private quick-private gui-private UbuntuGestures
SOURCES += \
    GestureTest.cpp \
    tst_directionaldragarea.cpp
HEADERS += GestureTest.h

OTHER_FILES += \
    DownwardsLauncher.qml \
    UpwardsLauncher.qml \
    LeftwardsLauncher.qml \
    RightwardsLauncher.qml \
    tst_directionaldragarea.qml
