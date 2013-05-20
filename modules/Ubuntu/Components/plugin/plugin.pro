include( ../../../../common.pri )

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0
}

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += qml quick quick-private dbus
CONFIG += qt plugin no_keywords

#needed by ItemStyleAttached
QT += qml-private core-private v8-private

#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h \
    themeengine.h \
    themeengine_p.h \
    themeloader_p.h \
    themesettings_p.h \
    stylecache_p.h \
    itemstyleattached.h \
    itemstyleattached_p.h \
    qmlthemeloader_p.h \
    i18n.h \
    listener.h \
    ucscalingimageprovider.h \
    ucunits.h \
    ucqquickimageextension.h \
    quickutils.h \
    giconprovider.h \
    shapeitemtexture.h \
    shapeitem.h \
    inversemouseareatype.h \
    bottombarvisibilitycommunicator.h \
    qquickclipboard.h \
    qquickmimedata.h \
    qquickclipboard_p.h \
    selector_p.h \
    ucstyle.h \
    ucubuntuanimation.h \
    ucfontutils.h

SOURCES += plugin.cpp \
    themeengine.cpp \
    stylecache.cpp \
    themesettings.cpp \
    itemstyleattached.cpp \
    qmlthemeloader.cpp \
    i18n.cpp \
    listener.cpp \
    ucscalingimageprovider.cpp \
    ucunits.cpp \
    ucqquickimageextension.cpp \
    quickutils.cpp \
    giconprovider.cpp \
    shapeitem.cpp \
    inversemouseareatype.cpp \
    bottombarvisibilitycommunicator.cpp \
    qquickclipboard.cpp \
    qquickmimedata.cpp \
    selector.cpp \
    ucstyle.cpp \
    ucubuntuanimation.cpp \
    ucfontutils.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
