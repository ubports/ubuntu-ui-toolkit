TEMPLATE=lib
TARGET=Gestures

QT *= core-private gui-private qml qml-private quick quick-private

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUGESTURES_LIBRARY

HEADERS += $$PWD/CandidateInactivityTimer.h \
           $$PWD/DebugHelpers.h \
           $$PWD/Timer.h \
           $$PWD/TimeSource.h \
           $$PWD/TouchOwnershipEvent.h \
           $$PWD/TouchRegistry.h \
           $$PWD/UnownedTouchEvent.h \
           $$PWD/UbuntuGesturesGlobal.h \
           $$PWD/Pool.h \

SOURCES += $$PWD/CandidateInactivityTimer.cpp \
           $$PWD/DebugHelpers.cpp \
           $$PWD/Timer.cpp \
           $$PWD/TimeSource.cpp \
           $$PWD/TouchOwnershipEvent.cpp \
           $$PWD/TouchRegistry.cpp \
           $$PWD/UnownedTouchEvent.cpp \
