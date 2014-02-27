TEMPLATE = lib
TARGET = ../UbuntuPerformanceMetrics
QT += qml quick
CONFIG += qt plugin no_keywords

QMAKE_CXXFLAGS += -Werror

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.PerformanceMetrics

# Input
SOURCES += \
    upmplugin.cpp \
    upmgraphmodel.cpp \
    upmtexturefromimage.cpp \
    upmrenderingtimes.cpp \
    upmcpuusage.cpp \
    rendertimer.cpp

HEADERS += \
    upmplugin.h \
    upmgraphmodel.h \
    upmtexturefromimage.h \
    upmrenderingtimes.h \
    upmcpuusage.h \
    rendertimer.h

contains(QT_CONFIG, opengles2) {
    CONFIG += egl
}

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
