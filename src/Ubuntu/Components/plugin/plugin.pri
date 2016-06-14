QT *= core-private qml qml-private quick quick-private gui-private dbus svg UbuntuGestures UbuntuGestures_private UbuntuToolkit

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

CONFIG += no_keywords c++11

#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_USE_QSTRINGBUILDER

INCLUDEPATH+=$$PWD

HEADERS += $$PWD/plugin.h \
    $$PWD/ucnamespace.h

SOURCES += $$PWD/plugin.cpp \
    $$PWD/ucnamespace.cpp


RESOURCES += \
    $$PWD/plugin.qrc

OTHER_FILES += \
    $$PWD/shaders/shape.vert \
    $$PWD/shaders/shape.frag \
    $$PWD/shaders/shape_no_dfdy.frag \
    $$PWD/shaders/shapeoverlay.vert \
    $$PWD/shaders/shapeoverlay.frag \
    $$PWD/shaders/shapeoverlay_no_dfdy.frag \
    $$PWD/privates/shaders/frame.vert \
    $$PWD/privates/shaders/frame.frag
