unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0 dbus-1 libnih-dbus
}

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += core-private qml qml-private quick quick-private gui-private dbus svg

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

CONFIG += qt plugin no_keywords

QMAKE_CXXFLAGS += -Werror

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
    shapeitemtexture.h \
    shapeitem.h \
    inversemouseareatype.h \
    qquickclipboard.h \
    qquickmimedata.h \
    qquickclipboard_p.h \
    ucubuntuanimation.h \
    ucfontutils.h \
    ucapplication.h \
    ucarguments.h \
    ucargument.h \
    ucalarm.h \
    ucalarm_p.h \
    alarmmanager_p_p.h \
    alarmmanager_p.h \
    ucalarmmodel.h \
    unitythemeiconprovider.h \
    adapters/alarmsadapter_p.h \
    ucstatesaver.h \
    sortbehavior.h \
    filterbehavior.h \
    sortfiltermodel.h \
    statesaverbackend_p.h \
    ucstatesaver_p.h \
    ucurihandler.h \
    ucinversemouse.h \
    ucmouse.h \
    unixsignalhandler_p.h \
    ucstyleditembase.h \
    ucstyleditembase_p.h \
    ucaction.h \
    ucactioncontext.h \
    ucactionmanager.h \
    adapters/actionsproxy_p.h \
    uclistitem.h \
    uclistitem_p.h \
    uclistitemactions.h \
    uclistitemactions_p.h \
    propertychange_p.h \
    uclistitemstyle.h

SOURCES += plugin.cpp \
    uctheme.cpp \
    ucthemesettings.cpp \
    i18n.cpp \
    listener.cpp \
    ucscalingimageprovider.cpp \
    ucunits.cpp \
    ucqquickimageextension.cpp \
    quickutils.cpp \
    shapeitem.cpp \
    inversemouseareatype.cpp \
    qquickclipboard.cpp \
    qquickmimedata.cpp \
    ucubuntuanimation.cpp \
    ucfontutils.cpp \
    ucapplication.cpp \
    ucarguments.cpp \
    ucargument.cpp \
    ucalarm.cpp \
    alarmmanager_p.cpp \
    ucalarmmodel.cpp \
    unitythemeiconprovider.cpp \
    ucstatesaver.cpp \
    sortbehavior.cpp \
    filterbehavior.cpp \
    sortfiltermodel.cpp \
    statesaverbackend_p.cpp \
    ucurihandler.cpp \
    ucmousefilters.cpp \
    unixsignalhandler_p.cpp \
    ucstyleditembase.cpp \
    ucaction.cpp \
    ucactioncontext.cpp \
    ucactionmanager.cpp \
    adapters/actionsproxy_p.cpp \
    uclistitem.cpp \
    uclistitemactions.cpp \
    uclistitemactionsattached.cpp \
    uclistitemattached.cpp \
    propertychange_p.cpp \
    uclistitemstyle.cpp \
    uclistitemselect.cpp \
    uclistitemdrag.cpp

# adapters
SOURCES += adapters/alarmsadapter_organizer.cpp

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target
