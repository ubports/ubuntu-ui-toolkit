include( ../../../../qtversioncheck.pri )

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += qml quick quick-private
CONFIG += qt plugin no_keywords

#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

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
    qmlthemeloader_p.h \
    i18n.h \
    listener.h \
    ucscalingimageprovider.h \
    ucunits.h \
    ucqquickimageextension.h \
    quickutils.h

SOURCES += plugin.cpp \
    themeengine.cpp \
    suffixtree.cpp \
    themesettings.cpp \
    itemstyleattached.cpp \
    rule.cpp \
    qmlloader.cpp \
    qmlthemeloader.cpp \
    i18n.cpp \
    listener.cpp \
    ucscalingimageprovider.cpp \
    ucunits.cpp \
    ucqquickimageextension.cpp \
    quickutils.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

