TEMPLATE=lib
TARGET=UbuntuMetrics
QT += core-private gui-private quick-private
CONFIG += dll no_keywords c++11
INCLUDEPATH += $$PWD/..  # To be able to include <UbuntuMetrics/*.h> in public headers.

contains(QT_CONFIG, opengles2) {
    CONFIG += egl
    DEFINES += MESA_EGL_NO_X11_HEADERS
}

load(qt_build_config)
load(ubuntu_qt_module)

PUBLIC_HEADERS = \
    $$PWD/ubuntumetricsglobal.h \
    $$PWD/events.h \
    $$PWD/logger.h \
    $$PWD/applicationmonitor.h

HEADERS += \
    $${PUBLIC_HEADERS} \
    $$PWD/ubuntumetricsglobal_p.h \
    $$PWD/bitmaptextfont_p.h \
    $$PWD/events_p.h \
    $$PWD/logger_p.h \
    $$PWD/gputimer_p.h \
    $$PWD/bitmaptext_p.h \
    $$PWD/overlay_p.h \
    $$PWD/applicationmonitor_p.h

SOURCES += \
    $$PWD/events.cpp \
    $$PWD/logger.cpp \
    $$PWD/gputimer.cpp \
    $$PWD/bitmaptext.cpp \
    $$PWD/overlay.cpp \
    $$PWD/applicationmonitor.cpp
