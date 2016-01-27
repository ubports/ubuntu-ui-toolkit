include(../test-include.pri)
QT += core-private qml-private quick-private gui-private UbuntuGestures UbuntuGestures_private
SOURCES += \
    GestureTest.cpp \
    tst_swipearea.cpp
HEADERS += GestureTest.h

OTHER_FILES += \
    DownwardsLauncher.qml \
    UpwardsLauncher.qml \
    LeftwardsLauncher.qml \
    RightwardsLauncher.qml \
    tst_swipearea.qml
