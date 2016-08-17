TEMPLATE=lib
TARGET=UbuntuToolkit

QT *= core-private gui-private qml qml-private quick quick-private testlib

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0 dbus-1 libnih libnih-dbus
}
QT *= dbus svg UbuntuGestures UbuntuGestures_private UbuntuMetrics

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
    colorutils_p.h \
    ubuntutoolkitglobal.h \
    tree_p.h \
    asyncloader_p.h \
    asyncloader_p_p.h \
    mousetouchadaptor_p.h \
    mousetouchadaptor_p_p.h \
    propertychange_p.h \
    ubuntutoolkitmodule.h

SOURCES += \ 
    colorutils.cpp \
    tree.cpp \
    asyncloader.cpp \
    mousetouchadaptor.cpp \
    propertychange.cpp \
    ubuntutoolkitmodule.cpp

HEADERS += \
    uctheme_p.h \
    i18n_p.h \
    listener_p.h \
    ucscalingimageprovider_p.h \
    ucunits_p.h \
    ucqquickimageextension_p.h \
    quickutils_p.h \
    ucubuntushapetextures_p.h \
    ucubuntushape_p.h \
    ucubuntushapeoverlay_p.h \
    ucproportionalshape_p.h \
    inversemouseareatype_p.h \
    qquickclipboard_p.h \
    qquickmimedata_p.h \
    qquickclipboard_p_p.h \
    ucubuntuanimation_p.h \
    ucfontutils_p.h \
    ucapplication_p.h \
    ucarguments_p.h \
    ucargument_p.h \
    ucalarm_p.h \
    ucalarm_p_p.h \
    alarmmanager_p_p.h \
    alarmmanager_p.h \
    ucalarmmodel_p.h \
    unitythemeiconprovider_p.h \
    adapters/alarmsadapter_p.h \
    ucstatesaver_p.h \
    sortbehavior_p.h \
    filterbehavior_p.h \
    sortfiltermodel_p.h \
    statesaverbackend_p.h \
    ucstatesaver_p_p.h \
    ucurihandler_p.h \
    ucinversemouse_p.h \
    ucmouse_p.h \
    unixsignalhandler_p.h \
    ucstyleditembase_p.h \
    ucstyleditembase_p_p.h \
    ucaction_p.h \
    ucactioncontext_p.h \
    ucactionmanager_p.h \
    adapters/actionsproxy_p.h \
    adapters/dbuspropertywatcher_p.h \
    uclistitem_p.h \
    uclistitem_p_p.h \
    uclistitemactions_p.h \
    uclistitemactions_p_p.h \
    uclistitemstyle_p.h \
    ucserviceproperties_p.h \
    ucserviceproperties_p_p.h \
    privates/listitemdragarea_p.h \
    privates/listitemdraghandler_p.h \
    privates/listitemselection_p.h \
    ucdeprecatedtheme_p.h \
    ucdefaulttheme_p.h \
    ucstylehints_p.h \
    ucslotslayout_p.h \
    ucslotslayout_p_p.h \
    livetimer_p.h \
    livetimer_p_p.h \
    timeutils_p.h \
    ucactionitem_p.h \
    ucactionitem_p_p.h \
    uchaptics_p.h \
    ucabstractbutton_p.h \
    ucabstractbutton_p_p.h \
    ucmargins_p.h \
    ucthemingextension_p.h \
    ucheader_p.h \
    uclabel_p.h \
    uclistitemlayout_p.h \
    privates/threelabelsslot_p.h \
    ucimportversionchecker_p.h \
    ucbottomedgehint_p.h \
    ucbottomedgehint_p_p.h \
    ucmathutils_p.h \
    ucbottomedge_p.h \
    ucbottomedge_p_p.h \
    ucbottomedgestyle_p.h \
    ucbottomedgeregion_p.h \
    ucpagetreenode_p.h \
    ucpagetreenode_p_p.h \
    ucmainviewbase_p.h \
    ucmainviewbase_p_p.h \
    ucperformancemonitor_p.h \
    privates/listviewextensions_p.h \
    privates/frame_p.h \
    privates/ucpagewrapper_p.h \
    privates/ucpagewrapper_p_p.h \
    privates/ucpagewrapperincubator_p.h \
    privates/appheaderbase_p.h \
    label_p.h \
    ucbottomedgeregion_p_p.h \
    privates/ucscrollbarutils_p.h

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
    privates/appheaderbase.cpp \
    privates/ucscrollbarutils.cpp

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
