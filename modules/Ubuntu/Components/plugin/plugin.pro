unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0
}

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += qml quick quick-private dbus
CONFIG += qt plugin no_keywords

CONFIG(debug) {
  QMAKE_CXXFLAGS_DEBUG += -Werror
}

# QOrganizer
QT += organizer

#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h \
    uctheme.h \
    ucthemesettings.h \
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
    ucubuntuanimation.h \
    ucfontutils.h \
    ucarguments.h \
    ucargument.h \
    ucalarm.h \
    ucalarm_p.h \
    ucalarms.h \
    ucalarms_p.h

SOURCES += plugin.cpp \
    uctheme.cpp \
    ucthemesettings.cpp \
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
    ucubuntuanimation.cpp \
    ucfontutils.cpp \
    ucarguments.cpp \
    ucargument.cpp \
    ucalarm.cpp \
    ucalarms.cpp

# adapters
SOURCES += adapters/alarmsadapter_organizer.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
