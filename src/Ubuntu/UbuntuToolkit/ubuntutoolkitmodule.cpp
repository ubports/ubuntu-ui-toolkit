/*
 * Copyright 2016 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ubuntutoolkitmodule.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExtensionPlugin>
#include <QtQuick/private/qquickimagebase_p.h>
#include <QDBusConnection>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>

#include <ucunits_p.h>
#include <quickutils_p.h>
#include <i18n_p.h>
#include <ucapplication_p.h>
#include <ucfontutils_p.h>
#include <uctheme_p.h>
#include <ucdeprecatedtheme_p.h>
#include <listener_p.h>
#include <ucfontutils_p.h>
#include <ucmathutils_p.h>
#include <ucmargins_p.h>
#include <ucmainviewbase_p.h>
#include <ucabstractbutton_p.h>
#include <ucaction_p.h>
#include <ucactioncontext_p.h>
#include <ucactionitem_p.h>
#include <ucactionmanager_p.h>
#include <ucalarm_p.h>
#include <ucalarmmodel_p.h>
#include <ucarguments_p.h>
#include <ucargument_p.h>
#include <ucbottomedgehint_p.h>
#include <ucbottomedgeregion_p.h>
#include <ucbottomedge_p.h>
#include <uchaptics_p.h>
#include <ucheader_p.h>
#include <ucinversemouse_p.h>
#include <ucmouse_p.h>
#include <uclistitem_p.h>
#include <uclistitemactions_p.h>
#include <uclistitemlayout_p.h>
#include <uclabel_p.h>
#include <ucpagetreenode_p.h>
#include <ucperformancemonitor_p.h>
#include <ucproportionalshape_p.h>
#include <ucscalingimageprovider_p.h>
#include <ucserviceproperties_p.h>
#include <ucslotslayout_p.h>
#include <ucstatesaver_p.h>
#include <ucstyleditembase_p.h>
#include <ucstylehints_p.h>
#include <uctheme_p.h>
#include <ucubuntuanimation_p.h>
#include <ucubuntushapeoverlay_p.h>
#include <ucubuntushape_p.h>
#include <ucurihandler_p.h>
#include <ucqquickimageextension_p.h>
#include <inversemouseareatype_p.h>
#include <qquickclipboard_p.h>
#include <qquickmimedata_p.h>
#include <colorutils_p.h>
#include <tree_p.h>
#include <unitythemeiconprovider_p.h>
#include <sortbehavior_p.h>
#include <sortfiltermodel_p.h>
#include <livetimer_p.h>
#include <privates/frame_p.h>
#include <privates/ucpagewrapper_p.h>
#include <privates/appheaderbase_p.h>

// styles
#include <ucbottomedgestyle_p.h>

#include <UbuntuMetrics/applicationmonitor.h>

UT_NAMESPACE_BEGIN

const char *EngineProperty("__ubuntu_toolkit_plugin_data");

/******************************************************************************
 * UbuntuToolkitModule
 */
UbuntuToolkitModule* UbuntuToolkitModule::create(QQmlEngine *engine, const QUrl &baseUrl)
{
    if (!engine) {
        return Q_NULLPTR;
    }
    UbuntuToolkitModule *data = new UbuntuToolkitModule(engine);
    data->m_baseUrl = QUrl(baseUrl.toString() + '/');
    engine->setProperty(EngineProperty, QVariant::fromValue(data));
    return data;
}

void UbuntuToolkitModule::registerWindowContextProperty()
{
    setWindowContextProperty(QGuiApplication::focusWindow());

    // listen to QGuiApplication::focusWindowChanged
    /* Ensure that setWindowContextProperty is called in the same thread (the
       main thread) otherwise it segfaults. Reference:
       https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205556
    */
    QGuiApplication* application = static_cast<QGuiApplication*>(QCoreApplication::instance());
    QObject::connect(application, SIGNAL(focusWindowChanged(QWindow*)),
                     this, SLOT(setWindowContextProperty(QWindow*)),
                     Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

}

void UbuntuToolkitModule::setWindowContextProperty(QWindow* focusWindow)
{
    QQuickView* view = qobject_cast<QQuickView*>(focusWindow);

    if (view != NULL) {
        view->rootContext()->setContextProperty("window", view);
    }
}

void UbuntuToolkitModule::initializeContextProperties(QQmlEngine *engine)
{
    UCUnits::instance(engine);
    QuickUtils::instance(engine);
    UbuntuI18n::instance(engine);
    UCApplication::instance(engine);
    UCFontUtils::instance(engine);
    UCTheme::defaultTheme(engine);

    QQmlContext* context = engine->rootContext();

    // register root object watcher that sets a global property with the root object
    // that can be accessed from any object
    context->setContextProperty("QuickUtils", QuickUtils::instance());

    UCDeprecatedTheme::registerToContext(context);

    context->setContextProperty("i18n", UbuntuI18n::instance());
    ContextPropertyChangeListener *i18nChangeListener =
        new ContextPropertyChangeListener(context, "i18n");
    QObject::connect(UbuntuI18n::instance(), SIGNAL(domainChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));
    QObject::connect(UbuntuI18n::instance(), SIGNAL(languageChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));

    // We can't use 'Application' because it exists (undocumented)
    context->setContextProperty("UbuntuApplication", UCApplication::instance());
    ContextPropertyChangeListener *applicationChangeListener =
        new ContextPropertyChangeListener(context, "UbuntuApplication");
    QObject::connect(UCApplication::instance(), SIGNAL(applicationNameChanged()),
                     applicationChangeListener, SLOT(updateContextProperty()));
    // Give the application object access to the engine
    UCApplication::instance()->setContext(context);

    context->setContextProperty("units", UCUnits::instance());
    ContextPropertyChangeListener *unitsChangeListener =
        new ContextPropertyChangeListener(context, "units");
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     unitsChangeListener, SLOT(updateContextProperty()));

    // register FontUtils
    context->setContextProperty("FontUtils", UCFontUtils::instance());
    ContextPropertyChangeListener *fontUtilsListener =
        new ContextPropertyChangeListener(context, "FontUtils");
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     fontUtilsListener, SLOT(updateContextProperty()));
}

void UbuntuToolkitModule::registerTypesToVersion(const char *uri, int major, int minor)
{
    qmlRegisterType<UCAction>(uri, major, minor, "Action");
    qmlRegisterType<UCActionContext>(uri, major, minor, "ActionContext");
    qmlRegisterUncreatableType<UCApplication>(uri, major, minor, "UCApplication", "Not instantiable");
    qmlRegisterType<UCActionManager>(uri, major, minor, "ActionManager");
    qmlRegisterUncreatableType<UCFontUtils>(uri, major, minor, "UCFontUtils", "Not instantiable");
    qmlRegisterType<UCStyledItemBase>(uri, major, minor, "StyledItem");
    qmlRegisterUncreatableType<UbuntuI18n>(uri, major, minor, "i18n", "Singleton object");
    qmlRegisterExtendedType<QQuickImageBase, UCQQuickImageExtension>(uri, major, minor, "QQuickImageBase");
    qmlRegisterUncreatableType<UCUnits>(uri, major, minor, "UCUnits", "Not instantiable");
    qmlRegisterType<UCUbuntuShape>(uri, major, minor, "UbuntuShape");
    // FIXME/DEPRECATED: Shape is exported for backwards compatibility only
    qmlRegisterType<UCUbuntuShape>(uri, major, minor, "Shape");
    qmlRegisterType<InverseMouseAreaType>(uri, major, minor, "InverseMouseArea");
    qmlRegisterType<QQuickMimeData>(uri, major, minor, "MimeData");
    qmlRegisterSimpleSingletonType<QQuickClipboard>(uri, major, minor, "Clipboard");
    qmlRegisterSimpleSingletonType<UCUbuntuAnimation>(uri, major, minor, "UbuntuAnimation");
    qmlRegisterType<UCArguments>(uri, major, minor, "Arguments");
    qmlRegisterType<UCArgument>(uri, major, minor, "Argument");
    qmlRegisterType<QQmlPropertyMap>();
    qmlRegisterType<UCAlarm>(uri, major, minor, "Alarm");
    qmlRegisterType<UCAlarmModel>(uri, major, minor, "AlarmModel");
    qmlRegisterType<UCStateSaver>(uri, major, minor, "StateSaver");
    qmlRegisterType<UCStateSaverAttached>();
    qmlRegisterSimpleSingletonType<UCUriHandler>(uri, major, minor, "UriHandler");
    qmlRegisterType<UCMouse>(uri, major, minor, "Mouse");
    qmlRegisterType<UCInverseMouse>(uri, major, minor, "InverseMouse");
    qmlRegisterType<UCActionItem>(uri, major, minor, "ActionItem");
    qmlRegisterSimpleSingletonType<UCHaptics>(uri, major, minor, "Haptics");
    qmlRegisterSimpleSingletonType<UCMathUtils>(uri, major, minor, "MathUtils");
    qmlRegisterSimpleSingletonType<ColorUtils>(uri, major, minor, "ColorUtils");
}

/*
 * public API
 */
UbuntuToolkitModule::UbuntuToolkitModule(QObject *parent)
    : QObject(parent)
{
}

QUrl UbuntuToolkitModule::baseUrl(QQmlEngine *engine)
{
    if (!engine) {
        return QUrl();
    }
    UbuntuToolkitModule *data = engine->property(EngineProperty).value<UbuntuToolkitModule*>();
    return !data ? QUrl() : data->m_baseUrl;
}

void UbuntuToolkitModule::initializeModule(QQmlEngine *engine, const QUrl &pluginBaseUrl)
{
    UbuntuToolkitModule *module = create(engine, pluginBaseUrl);

    // Register private types.
    const char *privateUri = "Ubuntu.Components.Private";
    qmlRegisterType<UCFrame>(privateUri, 1, 3, "Frame");
    qmlRegisterType<UCPageWrapper>(privateUri, 1, 3, "PageWrapper");
    qmlRegisterType<UCAppHeaderBase>(privateUri, 1, 3, "AppHeaderBase");
    qmlRegisterType<Tree>(privateUri, 1, 3, "Tree");

    // allocate all context property objects prior we register them
    initializeContextProperties(engine);

    HapticsProxy::instance(engine);

    engine->addImageProvider(QLatin1String("scaling"), new UCScalingImageProvider);

    // register icon provider
    engine->addImageProvider(QLatin1String("theme"), new UnityThemeIconProvider);

    // Necessary for Screen.orientation (from import QtQuick.Window 2.0) to work
    QGuiApplication::primaryScreen()->setOrientationUpdateMask( Qt::ScreenOrientations(
            Qt::PortraitOrientation |
            Qt::LandscapeOrientation |
            Qt::InvertedPortraitOrientation |
            Qt::InvertedLandscapeOrientation));

    module->registerWindowContextProperty();

    // Application monitoring.
    const bool metricsOverlay = qEnvironmentVariableIsSet("UC_METRICS_OVERLAY");
    const QByteArray metricsLogging = qgetenv("UC_METRICS_LOGGING");
    if (metricsOverlay || !metricsLogging.isNull()) {
        UMApplicationMonitor::MonitorFlags flags = 0;
        if (!metricsLogging.isNull()) {
            UMLogger* logger = (metricsLogging.isEmpty() || metricsLogging == "stdout")
                ? new UMFileLogger(stdout) : new UMFileLogger(metricsLogging);
            if (logger->isOpen()) {
                flags |= UMApplicationMonitor::Logging;
                UMApplicationMonitor::installLogger(logger);
            } else {
                delete logger;
            }
        }
        if (metricsOverlay) {
            flags |= UMApplicationMonitor::Overlay;
        }
        UMApplicationMonitor::setFlags(flags);
        if (!UMApplicationMonitor::isActive()) {
            UMApplicationMonitor::start();
        }
    }

    // register performance monitor
    engine->rootContext()->setContextProperty("performanceMonitor", new UCPerformanceMonitor(engine));
}

void UbuntuToolkitModule::defineModule()
{
    const char *uri = "Ubuntu.Components";
    // register 0.1 for backward compatibility
    registerTypesToVersion(uri, 0, 1);
    registerTypesToVersion(uri, 1, 0);

    // register custom event
    ForwardedEvent::registerForwardedEvent();

    // register parent type so that properties can get/ set it
    qmlRegisterUncreatableType<QAbstractItemModel>(uri, 1, 1, "QAbstractItemModel", "Not instantiable");

    // register 1.1 only API
    qmlRegisterType<UCStyledItemBase, 1>(uri, 1, 1, "StyledItem");
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 1, 1, "SortFilterModel");
    qmlRegisterUncreatableType<FilterBehavior>(uri, 1, 1, "FilterBehavior", "Not instantiable");
    qmlRegisterUncreatableType<SortBehavior>(uri, 1, 1, "SortBehavior", "Not instantiable");
    qmlRegisterType<UCServiceProperties, 1>(uri, 1, 1, "ServiceProperties");

    // register 1.2 only API
    qmlRegisterType<UCListItem>(uri, 1, 2, "ListItem");
    qmlRegisterType<UCListItemDivider>();
    qmlRegisterUncreatableType<UCSwipeEvent>(uri, 1, 2, "SwipeEvent", "This is an event object.");
    qmlRegisterUncreatableType<UCDragEvent>(uri, 1, 2, "ListItemDrag", "This is an event object");
    qmlRegisterType<UCListItemActions>(uri, 1, 2, "ListItemActions");
    qmlRegisterUncreatableType<UCViewItemsAttached>(uri, 1, 2, "ViewItems", "Not instantiable");
    qmlRegisterType<UCUbuntuShape, 1>(uri, 1, 2, "UbuntuShape");
    qmlRegisterType<UCUbuntuShapeOverlay>(uri, 1, 2, "UbuntuShapeOverlay");

    // register 1.3 API
    qmlRegisterType<UCListItem, 1>(uri, 1, 3, "ListItem");
    qmlRegisterType<UCListItemExpansion>();
    qmlRegisterType<UCTheme>(uri, 1, 3, "ThemeSettings");
    qmlRegisterType<UCStyledItemBase, 2>(uri, 1, 3, "StyledItem");
    qmlRegisterType<UCStyledItemBase, 2>(uri, 1, 3, "StyledItem");
    qmlRegisterCustomType<UCStyleHints>(uri, 1, 3, "StyleHints", new UCStyleHintsParser);
    qmlRegisterType<UCAction, 1>(uri, 1, 3, "Action");
    qmlRegisterType<UCSlotsLayout>(uri, 1, 3, "SlotsLayout");
    qmlRegisterType<UCUbuntuShape, 2>(uri, 1, 3, "UbuntuShape");
    qmlRegisterType<UCProportionalShape>(uri, 1, 3, "ProportionalShape");
    qmlRegisterType<LiveTimer>(uri, 1, 3, "LiveTimer");
    qmlRegisterType<UCAbstractButton>(uri, 1, 3, "AbstractButton");
    qmlRegisterType<UCMargins>();
    qmlRegisterUncreatableType<UCSlotsAttached>(uri, 1, 3, "SlotsAttached", "Not instantiable");
    qmlRegisterUncreatableType<UCSlotsLayoutPadding>(uri, 1, 3, "SlotsLayoutPadding", "Not instantiable");
    qmlRegisterType<UCListItemLayout>(uri, 1, 3, "ListItemLayout");
    qmlRegisterType<UCHeader>(uri, 1, 3, "Header");
    qmlRegisterType<UCLabel>(uri, 1, 3, "Label");
    qmlRegisterType<UCBottomEdgeHint>(uri, 1, 3, "BottomEdgeHint");
    qmlRegisterType<UCBottomEdge>(uri, 1, 3, "BottomEdge");
    qmlRegisterType<UCBottomEdgeRegion>(uri, 1, 3, "BottomEdgeRegion");
    qmlRegisterType<UCPageTreeNode>(uri, 1, 3, "PageTreeNode");
    qmlRegisterType<UCPopupContext>(uri, 1, 3, "PopupContext");
    qmlRegisterType<UCMainViewBase>(uri, 1, 3, "MainViewBase");
}

void UbuntuToolkitModule::undefineModule()
{
    // nothing yet
}

/******************************************************************************
 * Styles module
 */
void UbuntuStylesModule::defineModule(const char *uri)
{
    // 1.2 styles
    qmlRegisterType<UCListItemStyle>(uri, 1, 2, "ListItemStyle");

    // 1.3 styles
    qmlRegisterType<UCListItemStyle, 1>(uri, 1, 3, "ListItemStyle");
    qmlRegisterType<UCBottomEdgeStyle>(uri, 1, 3, "BottomEdgeStyle");
}

void UbuntuStylesModule::undefineModule()
{
    // nothing yet
}

/******************************************************************************
 * Labs module
 */
void UbuntuLabsModule::initializeModule(QQmlEngine *engine, QQmlExtensionPlugin *plugin)
{
    Q_UNUSED(engine);
    Q_UNUSED(plugin);
}

void UbuntuLabsModule::defineModule(const char *uri)
{
    Q_UNUSED(uri);
    // a fake component so we can have the module types file created
    qmlRegisterType<QObject>(uri, 1, 0, "ZiObject");
}

void UbuntuLabsModule::undefineModule()
{
    // nothing yet
}

UT_NAMESPACE_END
