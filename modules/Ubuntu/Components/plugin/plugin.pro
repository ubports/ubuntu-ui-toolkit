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
    units.h \
    scalingimageprovider.h \
    qquickimageextension.h

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
    units.cpp \
    scalingimageprovider.cpp \
    qquickimageextension.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

