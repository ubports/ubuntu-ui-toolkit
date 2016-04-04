TEMPLATE=lib
TARGET=UbuntuToolkit

QT *= core-private gui-private qml qml-private quick quick-private testlib

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUTOOLKIT_LIBRARY

load(qt_build_config)
load(ubuntu_qt_module)

HEADERS += \ 
    colorutils.h \
    ubuntutoolkitglobal.h \
    tree.h \
    asyncloader.h \
    asyncloader_p.h \
    mousetouchadaptor.h

SOURCES += \ 
    colorutils.cpp \
    tree.cpp \
    asyncloader.cpp \
    mousetouchadaptor.cpp
