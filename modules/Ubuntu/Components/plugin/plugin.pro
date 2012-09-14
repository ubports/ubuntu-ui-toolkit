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
    styleditem_p.h \
    style_p.h

SOURCES += plugin.cpp \
    style.cpp \
    themeengine.cpp \
    styleditem.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
