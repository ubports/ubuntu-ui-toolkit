include( ../../../../qtversioncheck.pri )

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += qml
CONFIG += qt plugin no_keywords

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

# configures setting patterns for demo target; comment out to use
# release patterns
DEFINES += TARGET_DEMO

HEADERS += plugin.h \
    style.h \
    themeengine.h \
    styleditem.h \
    styleditem_p.h \
    style_p.h \
    themeengine_p.h \
    qmlthemeloader_p.h \
    qthmthemeloader_p.h \
    themeloader_p.h

SOURCES += plugin.cpp \
    style.cpp \
    themeengine.cpp \
    styleditem.cpp \
    qmlthemeloader.cpp \
    suffixtree.cpp \
    themesettings.cpp \
    qthmthemeloader.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
