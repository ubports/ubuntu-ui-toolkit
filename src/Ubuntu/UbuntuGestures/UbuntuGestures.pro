TEMPLATE=lib
TARGET=UbuntuGestures

QT *= core-private gui-private qml qml-private quick quick-private

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUGESTURES_LIBRARY

load(qt_build_config)
load(ubuntu_qt_module)

HEADERS += candidateinactivitytimer.h \
           debughelpers.h \
           timer.h \
           timesource.h \
           touchownershipevent.h \
           touchregistry.h \
           unownedtouchevent.h \
           ubuntugesturesglobal.h \
           pool.h \
           ucswipearea_p.h \
           ucswipearea_p_p.h \
           damper_p.h \


SOURCES += candidateinactivitytimer.cpp \
           debughelpers.cpp \
           timer.cpp \
           timesource.cpp \
           touchownershipevent.cpp \
           touchregistry.cpp \
           unownedtouchevent.cpp \
           ucswipearea.cpp \
           damper.cpp

