QT *= core-private qml qml-private quick quick-private gui-private dbus svg \
      UbuntuGestures UbuntuGestures_private UbuntuToolkit UbuntuToolkit_private

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

CONFIG += no_keywords c++11

INCLUDEPATH+=$$PWD

HEADERS += $$PWD/plugin.h \
    $$PWD/ucnamespace.h

SOURCES += $$PWD/plugin.cpp \
    $$PWD/ucnamespace.cpp
