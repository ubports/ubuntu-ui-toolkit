TEMPLATE=lib
TARGET=Gestures

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUGESTURES_LIBRARY

HEADERS += $$PWD\CandidateInactivityTimer.h \
           $$PWD\DebugHelpers.h \
           $$PWD\Timer.h \
           $$PWD\TimerSource.h \
           $$PWD\TouchOwnershipEvent.h \
           $$PWD\TouchRegistry.h \
           $$PWD\UnownedTouchEvent.h \
           $$PWD\UbuntuGesturesGlobal.h \
           $$PWD\Pool.h \

SOURCES += $$PWD\CandidateInactivityTimer.cpp \
           $$PWD\DebugHelpers.cpp \
           $$PWD\Timer.cpp \
           $$PWD\TimerSource.cpp \
           $$PWD\TouchOwnershipEvent.cpp \
           $$PWD\TouchRegistry.cpp \
           $$PWD\UnownedTouchEvent.cpp \
