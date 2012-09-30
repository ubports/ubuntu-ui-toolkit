TEMPLATE = lib
TARGET = ../UbuntuI18n
QT += declarative core gui
CONFIG += qt plugin no_keywords

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components.i18n

HEADERS += i18n.h

SOURCES += i18n.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

