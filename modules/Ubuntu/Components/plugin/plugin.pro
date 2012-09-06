TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += declarative core gui
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h \
    style.h \
    themeengine.h \
    styleditem.h \
    styleditem_p.h

SOURCES += plugin.cpp \
    style.cpp \
    themeengine.cpp \
    styleditem.cpp
