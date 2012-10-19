include( ../../../../qtversioncheck.pri )

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += qml quick
CONFIG += qt plugin no_keywords

#comment in the following line to remove traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

# configures setting patterns for demo target; comment out to use
# release patterns
DEFINES += TARGET_DEMO

HEADERS += plugin.h \
    themeengine.h \
    themeengine_p.h \
    themeloader_p.h \
    themesettings_p.h \
    suffixtree_p.h \
    itemstyleattached.h \
    itemstyleattached_p.h \
    rule.h \
    rule_p.h \
    qmlloader_p.h \
    qmlthemeloader_p.h

SOURCES += plugin.cpp \
    themeengine.cpp \
    suffixtree.cpp \
    themesettings.cpp \
    itemstyleattached.cpp \
    rule.cpp \
    qmlloader.cpp \
    qmlthemeloader.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

