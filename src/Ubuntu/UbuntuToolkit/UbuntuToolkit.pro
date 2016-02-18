TEMPLATE=lib
TARGET=UbuntuToolkit

QT *= core-private gui-private qml qml-private quick quick-private

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUTOOLKIT_LIBRARY

load(qt_build_config)
load(ubuntu_qt_module)

HEADERS += \ 
    colorutils.h \
    ubuntutoolkitglobal.h

SOURCES += \ 
    colorutils.cpp
