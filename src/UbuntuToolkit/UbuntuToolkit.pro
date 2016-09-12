TARGET = UbuntuToolkit
QT = core-private gui-private qml-private quick-private testlib dbus svg organizer \
     UbuntuGestures-private UbuntuMetrics

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0 dbus-1 libnih libnih-dbus
}

!contains(QT_ARCH, arm) {
    DEFINES += UBUNTUTOOLKIT_ENABLE_X11_TOUCH_EMULATION
    LIBS += -lX11 -lxcb -lXi
    SOURCES += mousetouchadaptor_x11.cpp
}

# Uncomment to compile out qDebug() calls.
# DEFINES += QT_NO_DEBUG_OUTPUT

HEADERS += \
    $$PWD/actionlist_p.h \
    $$PWD/adapters/actionsproxy_p.h \
    $$PWD/adapters/alarmsadapter_p.h \
    $$PWD/adapters/dbuspropertywatcher_p.h \
    $$PWD/alarmmanager_p.h \
    $$PWD/alarmmanager_p_p.h \
    $$PWD/asyncloader_p.h \
    $$PWD/asyncloader_p_p.h \
    $$PWD/colorutils_p.h \
    $$PWD/exclusivegroup_p.h \
    $$PWD/filterbehavior_p.h \
    $$PWD/i18n_p.h \
    $$PWD/inversemouseareatype_p.h \
    $$PWD/label_p.h \
    $$PWD/listener_p.h \
    $$PWD/livetimer_p.h \
    $$PWD/livetimer_p_p.h \
    $$PWD/menu_p.h \
    $$PWD/menu_p_p.h \
    $$PWD/menubar_p.h \
    $$PWD/menubar_p_p.h \
    $$PWD/menugroup_p.h \
    $$PWD/mousetouchadaptor_p.h \
    $$PWD/mousetouchadaptor_p_p.h \
    $$PWD/privates/appheaderbase_p.h \
    $$PWD/privates/frame_p.h \
    $$PWD/privates/listitemdragarea_p.h \
    $$PWD/privates/listitemdraghandler_p.h \
    $$PWD/privates/listitemselection_p.h \
    $$PWD/privates/listviewextensions_p.h \
    $$PWD/privates/splitviewhandler_p.h \
    $$PWD/privates/threelabelsslot_p.h \
    $$PWD/privates/ucpagewrapper_p.h \
    $$PWD/privates/ucpagewrapper_p_p.h \
    $$PWD/privates/ucpagewrapperincubator_p.h \
    $$PWD/privates/ucscrollbarutils_p.h \
    $$PWD/propertychange_p.h \
    $$PWD/qquickclipboard_p.h \
    $$PWD/qquickclipboard_p_p.h \
    $$PWD/qquickmimedata_p.h \
    $$PWD/quickutils_p.h \
    $$PWD/sortbehavior_p.h \
    $$PWD/sortfiltermodel_p.h \
    $$PWD/splitview_p.h \
    $$PWD/splitview_p_p.h \
    $$PWD/statesaverbackend_p.h \
    $$PWD/timeutils_p.h \
    $$PWD/tree_p.h \
    $$PWD/ubuntutoolkitglobal.h \
    $$PWD/ubuntutoolkitmodule.h \
    $$PWD/ucabstractbutton_p.h \
    $$PWD/ucabstractbutton_p_p.h \
    $$PWD/ucaction_p.h \
    $$PWD/ucactioncontext_p.h \
    $$PWD/ucactionitem_p.h \
    $$PWD/ucactionitem_p_p.h \
    $$PWD/ucactionmanager_p.h \
    $$PWD/ucalarm_p.h \
    $$PWD/ucalarm_p_p.h \
    $$PWD/ucalarmmodel_p.h \
    $$PWD/ucapplication_p.h \
    $$PWD/ucargument_p.h \
    $$PWD/ucarguments_p.h \
    $$PWD/ucbottomedge_p.h \
    $$PWD/ucbottomedge_p_p.h \
    $$PWD/ucbottomedgehint_p.h \
    $$PWD/ucbottomedgehint_p_p.h \
    $$PWD/ucbottomedgeregion_p.h \
    $$PWD/ucbottomedgeregion_p_p.h \
    $$PWD/ucbottomedgestyle_p.h \
    $$PWD/ucdefaulttheme_p.h \
    $$PWD/ucdeprecatedtheme_p.h \
    $$PWD/ucfontutils_p.h \
    $$PWD/uchaptics_p.h \
    $$PWD/ucheader_p.h \
    $$PWD/ucimportversionchecker_p.h \
    $$PWD/ucinversemouse_p.h \
    $$PWD/uclabel_p.h \
    $$PWD/uclistitem_p.h \
    $$PWD/uclistitem_p_p.h \
    $$PWD/uclistitemactions_p.h \
    $$PWD/uclistitemactions_p_p.h \
    $$PWD/uclistitemlayout_p.h \
    $$PWD/uclistitemstyle_p.h \
    $$PWD/ucmainviewbase_p.h \
    $$PWD/ucmainviewbase_p_p.h \
    $$PWD/ucmargins_p.h \
    $$PWD/ucmathutils_p.h \
    $$PWD/ucmouse_p.h \
    $$PWD/ucpagetreenode_p.h \
    $$PWD/ucpagetreenode_p_p.h \
    $$PWD/ucperformancemonitor_p.h \
    $$PWD/ucproportionalshape_p.h \
    $$PWD/ucqquickimageextension_p.h \
    $$PWD/ucscalingimageprovider_p.h \
    $$PWD/ucserviceproperties_p.h \
    $$PWD/ucserviceproperties_p_p.h \
    $$PWD/ucslotslayout_p.h \
    $$PWD/ucslotslayout_p_p.h \
    $$PWD/ucstatesaver_p.h \
    $$PWD/ucstatesaver_p_p.h \
    $$PWD/ucstyleditembase_p.h \
    $$PWD/ucstyleditembase_p_p.h \
    $$PWD/ucstylehints_p.h \
    $$PWD/uctheme_p.h \
    $$PWD/ucthemingextension_p.h \
    $$PWD/ucubuntuanimation_p.h \
    $$PWD/ucubuntushape_p.h \
    $$PWD/ucubuntushapeoverlay_p.h \
    $$PWD/ucubuntushapetextures_p.h \
    $$PWD/ucunits_p.h \
    $$PWD/ucurihandler_p.h \
    $$PWD/unitythemeiconprovider_p.h \
    $$PWD/unixsignalhandler_p.h

SOURCES += \
    $$PWD/actionlist.cpp \
    $$PWD/adapters/actionsproxy_p.cpp \
    $$PWD/adapters/alarmsadapter_organizer.cpp \
    $$PWD/adapters/dbuspropertywatcher_p.cpp \
    $$PWD/alarmmanager_p.cpp \
    $$PWD/asyncloader.cpp \
    $$PWD/colorutils.cpp \
    $$PWD/exclusivegroup.cpp \
    $$PWD/filterbehavior.cpp \
    $$PWD/i18n.cpp \
    $$PWD/inversemouseareatype.cpp \
    $$PWD/listener.cpp \
    $$PWD/livetimer.cpp \
    $$PWD/livetimer_p.cpp \
    $$PWD/menu.cpp \
    $$PWD/menubar.cpp \
    $$PWD/menugroup.cpp \
    $$PWD/mousetouchadaptor.cpp \
    $$PWD/privates/appheaderbase.cpp \
    $$PWD/privates/frame.cpp \
    $$PWD/privates/listitemdragarea.cpp \
    $$PWD/privates/listitemdraghandler.cpp \
    $$PWD/privates/listitemexpansion.cpp \
    $$PWD/privates/listitemselection.cpp \
    $$PWD/privates/listviewextensions.cpp \
    $$PWD/privates/splitviewhandler.cpp \
    $$PWD/privates/threelabelsslot_p.cpp \
    $$PWD/privates/ucpagewrapper.cpp \
    $$PWD/privates/ucpagewrapperincubator.cpp \
    $$PWD/privates/ucscrollbarutils.cpp \
    $$PWD/propertychange.cpp \
    $$PWD/qquickclipboard.cpp \
    $$PWD/qquickmimedata.cpp \
    $$PWD/quickutils.cpp \
    $$PWD/sortbehavior.cpp \
    $$PWD/sortfiltermodel.cpp \
    $$PWD/splitview.cpp \
    $$PWD/splitviewlayout.cpp \
    $$PWD/statesaverbackend_p.cpp \
    $$PWD/tree.cpp \
    $$PWD/ubuntutoolkitmodule.cpp \
    $$PWD/ucabstractbutton.cpp \
    $$PWD/ucaction.cpp \
    $$PWD/ucactioncontext.cpp \
    $$PWD/ucactionitem.cpp \
    $$PWD/ucactionmanager.cpp \
    $$PWD/ucalarm.cpp \
    $$PWD/ucalarmmodel.cpp \
    $$PWD/ucapplication.cpp \
    $$PWD/ucargument.cpp \
    $$PWD/ucarguments.cpp \
    $$PWD/ucbottomedge.cpp \
    $$PWD/ucbottomedgehint.cpp \
    $$PWD/ucbottomedgeregion.cpp \
    $$PWD/ucbottomedgestyle.cpp \
    $$PWD/ucdefaulttheme.cpp \
    $$PWD/ucdeprecatedtheme.cpp \
    $$PWD/ucfontutils.cpp \
    $$PWD/uchaptics.cpp \
    $$PWD/ucheader.cpp \
    $$PWD/ucimportversionchecker_p.cpp \
    $$PWD/uclabel.cpp \
    $$PWD/uclistitem.cpp \
    $$PWD/uclistitemactions.cpp \
    $$PWD/uclistitemlayout.cpp \
    $$PWD/uclistitemstyle.cpp \
    $$PWD/ucmainviewbase.cpp \
    $$PWD/ucmathutils.cpp \
    $$PWD/ucmousefilters.cpp \
    $$PWD/ucpagetreenode.cpp \
    $$PWD/ucperformancemonitor.cpp \
    $$PWD/ucproportionalshape.cpp \
    $$PWD/ucqquickimageextension.cpp \
    $$PWD/ucscalingimageprovider.cpp \
    $$PWD/ucserviceproperties.cpp \
    $$PWD/ucslotslayout.cpp \
    $$PWD/ucstatesaver.cpp \
    $$PWD/ucstyleditembase.cpp \
    $$PWD/ucstylehints.cpp \
    $$PWD/uctheme.cpp \
    $$PWD/ucthemingextension.cpp \
    $$PWD/ucubuntuanimation.cpp \
    $$PWD/ucubuntushape.cpp \
    $$PWD/ucubuntushapeoverlay.cpp \
    $$PWD/ucubuntushapetextures.cpp \
    $$PWD/ucunits.cpp \
    $$PWD/ucurihandler.cpp \
    $$PWD/ucviewitemsattached.cpp \
    $$PWD/unitythemeiconprovider.cpp \
    $$PWD/unixsignalhandler_p.cpp

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

load(ubuntu_qt_module)
