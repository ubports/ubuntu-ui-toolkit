TEMPLATE=lib
TARGET=UbuntuGestures

QT *= core-private gui-private qml qml-private quick quick-private

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUGESTURES_LIBRARY

load(qt_build_config)
load(ubuntu_qt_module)

HEADERS += candidateinactivitytimer_p.h \
           debughelpers_p.h \
           timer_p.h \
           timesource_p.h \
           touchownershipevent_p.h \
           touchregistry_p.h \
           unownedtouchevent_p.h \
           ubuntugesturesglobal.h \
           pool_p.h \
           ucswipearea_p.h \
           ucswipearea_p_p.h \
           damper_p.h \
           ubuntugesturesmodule.h


SOURCES += candidateinactivitytimer.cpp \
           debughelpers.cpp \
           timer.cpp \
           timesource.cpp \
           touchownershipevent.cpp \
           touchregistry.cpp \
           unownedtouchevent.cpp \
           ucswipearea.cpp \
           damper.cpp \
           ubuntugesturesmodule.cpp

