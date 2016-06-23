TEMPLATE=lib
TARGET=UbuntuToolkit

QT *= core-private gui-private qml qml-private quick quick-private testlib

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0 dbus-1 libnih libnih-dbus
}
QT *= dbus svg UbuntuGestures UbuntuGestures_private

# QOrganizer for Alarm API
QT *= organizer

!contains(QT_ARCH, arm) {
    DEFINES += UBUNTUTOOLKIT_ENABLE_X11_TOUCH_EMULATION
    LIBS += -lX11 -lxcb -lXi

    SOURCES += mousetouchadaptor_x11.cpp
}

CONFIG += dll no_keywords c++11

INCLUDEPATH+=$$PWD

DEFINES += UBUNTUTOOLKIT_LIBRARY
#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_USE_QSTRINGBUILDER

load(qt_build_config)
load(ubuntu_qt_module)

QMAKE_CXXFLAGS -= -Werror=deprecated-declarations
DEFINES -= QT_DEPRECATED_WARNINGS
DEFINES -= QT_ASCII_CAST_WARNINGS

HEADERS += \
    colorutils.h \
    ubuntutoolkitglobal.h \
    tree.h \
    asyncloader.h \
    asyncloader_p.h \
    mousetouchadaptor.h \
    mousetouchadaptor_p.h \
    propertychange.h \
    ubuntutoolkitmodule.h

SOURCES += \ 
    colorutils.cpp \
    tree.cpp \
    asyncloader.cpp \
    mousetouchadaptor.cpp \
    propertychange.cpp \
    ubuntutoolkitmodule.cpp

HEADERS += \
    uctheme.h \
    i18n.h \
    listener.h \
    ucscalingimageprovider.h \
    ucunits.h \
    ucqquickimageextension.h \
    quickutils.h \
    ucubuntushapetextures.h \
    ucubuntushape.h \
    ucubuntushapeoverlay.h \
    ucproportionalshape.h \
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
    adapters/dbuspropertywatcher_p.h \
    uclistitem.h \
    uclistitem_p.h \
    uclistitemactions.h \
    uclistitemactions_p.h \
    uclistitemstyle.h \
    ucserviceproperties.h \
    ucserviceproperties_p.h \
    privates/listitemdragarea.h \
    privates/listitemdraghandler.h \
    privates/listitemselection.h \
    ucdeprecatedtheme.h \
    ucdefaulttheme.h \
    ucstylehints.h \
    ucslotslayout.h \
    ucslotslayout_p.h \
    livetimer.h \
    livetimer_p.h \
    timeutils_p.h \
    ucactionitem.h \
    ucactionitem_p.h \
    uchaptics.h \
    ucabstractbutton.h \
    ucabstractbutton_p.h \
    ucmargins.h \
    ucthemingextension.h \
    ucheader.h \
    uclabel.h \
    uclistitemlayout.h \
    privates/threelabelsslot_p.h \
    ucimportversionchecker_p.h \
    ucbottomedgehint.h \
    ucbottomedgehint_p.h \
    ucmathutils.h \
    ucbottomedge.h \
    ucbottomedge_p.h \
    ucbottomedgestyle.h \
    ucbottomedgeregion.h \
    ucpagetreenode.h \
    ucpagetreenode_p.h \
    ucmainviewbase.h \
    ucmainviewbase_p.h \
    ucperformancemonitor.h \
    privates/listviewextensions.h \
    privates/frame.h \
    privates/ucpagewrapper.h \
    privates/ucpagewrapper_p.h \
    privates/ucpagewrapperincubator_p.h \
    privates/appheaderbase.h \
    label_p.h \
    ucbottomedgeregion_p.h

SOURCES += \
    uctheme.cpp \
    i18n.cpp \
    listener.cpp \
    ucscalingimageprovider.cpp \
    ucunits.cpp \
    ucqquickimageextension.cpp \
    quickutils.cpp \
    ucubuntushapetextures.cpp \
    ucubuntushape.cpp \
    ucubuntushapeoverlay.cpp \
    ucproportionalshape.cpp \
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
    adapters/dbuspropertywatcher_p.cpp \
    uclistitem.cpp \
    uclistitemactions.cpp \
    uclistitemstyle.cpp \
    ucviewitemsattached.cpp \
    ucserviceproperties.cpp \
    privates/listitemdragarea.cpp \
    privates/listitemdraghandler.cpp \
    privates/listitemexpansion.cpp \
    privates/listitemselection.cpp \
    ucdeprecatedtheme.cpp \
    ucdefaulttheme.cpp \
    ucstylehints.cpp \
    ucslotslayout.cpp \
    livetimer.cpp \
    livetimer_p.cpp \
    ucactionitem.cpp \
    uchaptics.cpp \
    ucabstractbutton.cpp \
    ucthemingextension.cpp \
    ucheader.cpp \
    uclabel.cpp \
    uclistitemlayout.cpp \
    privates/threelabelsslot_p.cpp \
    ucimportversionchecker_p.cpp \
    ucbottomedgehint.cpp \
    ucmathutils.cpp \
    ucbottomedge.cpp \
    ucbottomedgestyle.cpp \
    ucbottomedgeregion.cpp \
    ucpagetreenode.cpp \
    ucmainviewbase.cpp \
    ucperformancemonitor.cpp \
    privates/listviewextensions.cpp \
    privates/frame.cpp \
    privates/ucpagewrapper.cpp \
    privates/ucpagewrapperincubator.cpp \
    privates/appheaderbase.cpp

# adapters
SOURCES += $$PWD/adapters/alarmsadapter_organizer.cpp

#resources
RESOURCES += \
    $$PWD/resources.qrc

OTHER_FILES += \
    $$PWD/shaders/shape.vert \
    $$PWD/shaders/shape.frag \
    $$PWD/shaders/shape_no_dfdy.frag \
    $$PWD/shaders/shapeoverlay.vert \
    $$PWD/shaders/shapeoverlay.frag \
    $$PWD/shaders/shapeoverlay_no_dfdy.frag \
    $$PWD/privates/shaders/frame.vert \
    $$PWD/privates/shaders/frame.frag
