TEMPLATE = lib
TARGET = UbuntuComponents
QT += declarative core gui
CONFIG += qt plugin link_pkgconfig no_keywords

unix {
    CONFIG += link_pkgconfig
}

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h

SOURCES += plugin.cpp

installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
