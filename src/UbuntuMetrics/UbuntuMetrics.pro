TARGET = UbuntuMetrics
QT = core-private gui-private quick-private
LIBS += -ldl

contains(QT_CONFIG, opengles2) {
    CONFIG += egl
    DEFINES += MESA_EGL_NO_X11_HEADERS
}

linux {
    DEFINES += \
        LTTNG_PLUGIN_INSTALL_PATH=\\\"$$[QT_INSTALL_PLUGINS]/ubuntu/metrics/libumlttng.so\\\"
    DEFINES += LTTNG_PLUGIN_BUILD_PATH=\\\"$$OUT_PWD/lttng/libumlttng.so\\\"
}

HEADERS += \
    $$PWD/applicationmonitor.h \
    $$PWD/applicationmonitor_p.h \
    $$PWD/bitmaptext_p.h \
    $$PWD/bitmaptextfont_p.h \
    $$PWD/events.h \
    $$PWD/events_p.h \
    $$PWD/gputimer_p.h \
    $$PWD/logger.h \
    $$PWD/logger_p.h \
    $$PWD/overlay_p.h \
    $$PWD/ubuntumetricsglobal.h \
    $$PWD/ubuntumetricsglobal_p.h \

SOURCES += \
    $$PWD/applicationmonitor.cpp \
    $$PWD/bitmaptext.cpp \
    $$PWD/events.cpp \
    $$PWD/gputimer.cpp \
    $$PWD/logger.cpp \
    $$PWD/overlay.cpp \
    $$PWD/ubuntumetricsglobal.cpp

load(ubuntu_qt_module)
