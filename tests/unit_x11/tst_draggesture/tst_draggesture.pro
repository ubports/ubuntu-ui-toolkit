include(../test-include.pri)
QT += core-private qml-private quick-private gui-private
SOURCES += tst_draggesture.cpp \
           GestureTest.cpp
HEADERS += GestureTest.h

OTHER_FILES += \
    tst_draggesture.qml \
    DownwardsLauncher.qml \
    UpwardsLauncher.qml \
    LeftwardsLauncher.qml \
    RightwardsLauncher.qml \
