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

#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>

#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>
#include <QtQuick/private/qquickimagebase_p.h>
#include <QtDBus/QDBusConnection>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <UbuntuMetrics/applicationmonitor.h>

#include "actionlist_p.h"
#include "colorutils_p.h"
#include "exclusivegroup_p.h"
#include "i18n_p.h"
#include "inversemouseareatype_p.h"
#include "listener_p.h"
#include "livetimer_p.h"
#include "menu_p.h"
#include "menubar_p.h"
#include "menugroup_p.h"
#include "privates/appheaderbase_p.h"
#include "privates/frame_p.h"
#include "privates/ucpagewrapper_p.h"
#include "privates/ucscrollbarutils_p.h"
#include "qquickclipboard_p.h"
#include "qquickmimedata_p.h"
#include "quickutils_p.h"
#include "sortbehavior_p.h"
#include "sortfiltermodel_p.h"
#include "splitview_p.h"
#include "tree_p.h"
#include "ucabstractbutton_p.h"
#include "ucaction_p.h"
#include "ucactioncontext_p.h"
#include "ucactionitem_p.h"
#include "ucactionmanager_p.h"
#include "ucalarm_p.h"
#include "ucalarmmodel_p.h"
#include "ucapplication_p.h"
#include "ucargument_p.h"
#include "ucarguments_p.h"
#include "ucbottomedge_p.h"
#include "ucbottomedgehint_p.h"
#include "ucbottomedgeregion_p.h"
#include "ucbottomedgestyle_p.h"
#include "ucdeprecatedtheme_p.h"
#include "ucfontutils_p.h"
#include "ucfontutils_p.h"
#include "uchaptics_p.h"
#include "ucheader_p.h"
#include "ucinversemouse_p.h"
#include "uclabel_p.h"
#include "uclistitem_p.h"
#include "uclistitemactions_p.h"
#include "uclistitemlayout_p.h"
#include "ucmainviewbase_p.h"
#include "ucmargins_p.h"
#include "ucmathutils_p.h"
#include "ucmouse_p.h"
#include "ucpagetreenode_p.h"
#include "ucperformancemonitor_p.h"
#include "ucproportionalshape_p.h"
#include "ucqquickimageextension_p.h"
#include "ucscalingimageprovider_p.h"
#include "ucserviceproperties_p.h"
#include "ucslotslayout_p.h"
#include "ucstatesaver_p.h"
#include "ucstyleditembase_p.h"
#include "ucstylehints_p.h"
#include "uctheme_p.h"
#include "uctheme_p.h"
#include "ucubuntuanimation_p.h"
#include "ucubuntushape_p.h"
#include "ucubuntushapeoverlay_p.h"
#include "ucunits_p.h"
#include "ucurihandler_p.h"
#include "unitythemeiconprovider_p.h"

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
        view->rootContext()->setContextProperty(QStringLiteral("window"), view);
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
    context->setContextProperty(QStringLiteral("QuickUtils"), QuickUtils::instance());

    UCDeprecatedTheme::registerToContext(context);

    context->setContextProperty(QStringLiteral("i18n"), UbuntuI18n::instance());
    ContextPropertyChangeListener *i18nChangeListener =
        new ContextPropertyChangeListener(context, QStringLiteral("i18n"));
    QObject::connect(UbuntuI18n::instance(), SIGNAL(domainChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));
    QObject::connect(UbuntuI18n::instance(), SIGNAL(languageChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));

    // We can't use 'Application' because it exists (undocumented)
    context->setContextProperty(QStringLiteral("UbuntuApplication"), UCApplication::instance());
    ContextPropertyChangeListener *applicationChangeListener =
        new ContextPropertyChangeListener(context, QStringLiteral("UbuntuApplication"));
    QObject::connect(UCApplication::instance(), SIGNAL(applicationNameChanged()),
                     applicationChangeListener, SLOT(updateContextProperty()));
    // Give the application object access to the engine
    UCApplication::instance()->setContext(context);

    context->setContextProperty(QStringLiteral("units"), UCUnits::instance());
    ContextPropertyChangeListener *unitsChangeListener =
        new ContextPropertyChangeListener(context, QStringLiteral("units"));
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     unitsChangeListener, SLOT(updateContextProperty()));

    // register FontUtils
    context->setContextProperty(QStringLiteral("FontUtils"), UCFontUtils::instance());
    ContextPropertyChangeListener *fontUtilsListener =
        new ContextPropertyChangeListener(context, QStringLiteral("FontUtils"));
    QObject::connect(UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     fontUtilsListener, SLOT(updateContextProperty()));
}

void UbuntuToolkitModule::registerTypesToVersion(const char *uri, int major, int minor)
{
    qmlRegisterType<UCAction>(uri, major, minor, "Action");
    qmlRegisterType<UCActionContext>(uri, major, minor, "ActionContext");
    qmlRegisterUncreatableType<UCApplication>(
        uri, major, minor, "UCApplication", QStringLiteral("Not instantiable"));
    qmlRegisterType<UCActionManager>(uri, major, minor, "ActionManager");
    qmlRegisterUncreatableType<UCFontUtils>(
        uri, major, minor, "UCFontUtils", QStringLiteral("Not instantiable"));
    qmlRegisterType<UCStyledItemBase>(uri, major, minor, "StyledItem");
    qmlRegisterUncreatableType<UbuntuI18n>(
        uri, major, minor, "i18n", QStringLiteral("Singleton object"));
    qmlRegisterExtendedType<
        QQuickImageBase, UCQQuickImageExtension>(uri, major, minor, "QQuickImageBase");
    qmlRegisterUncreatableType<
        UCUnits>(uri, major, minor, "UCUnits", QStringLiteral("Not instantiable"));
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

    //FIXME: move to a more generic location, i.e StyledItem or QuickUtils
    qmlRegisterSimpleSingletonType<UCScrollbarUtils>(privateUri, 1, 3, "PrivateScrollbarUtils");

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
    UMApplicationMonitor* applicationMonitor = UMApplicationMonitor::instance();
    const QString metricsLoggingFilter =
        QString::fromLocal8Bit(qgetenv("UC_METRICS_LOGGING_FILTER"));
    if (!metricsLoggingFilter.isNull()) {
        QStringList filterList =
            metricsLoggingFilter.split(QStringLiteral(","), QString::SkipEmptyParts);
        UMApplicationMonitor::LoggingFilters filter = 0;
        const int size = filterList.size();
        for (int i = 0; i < size; ++i) {
            if (filterList[i] == QStringLiteral("*")) {
                filter |= UMApplicationMonitor::AllEvents;
                break;
            } else if (filterList[i] == QStringLiteral("window")) {
                filter |= UMApplicationMonitor::WindowEvent;
            } else if (filterList[i] == QStringLiteral("process")) {
                filter |= UMApplicationMonitor::ProcessEvent;
            } else if (filterList[i] == QStringLiteral("frame")) {
                filter |= UMApplicationMonitor::FrameEvent;
            } else if (filterList[i] == QStringLiteral("generic")) {
                filter |= UMApplicationMonitor::GenericEvent;
            }
        }
        applicationMonitor->setLoggingFilter(filter);
    }
    const QByteArray metricsLogging = qgetenv("UC_METRICS_LOGGING");
    if (!metricsLogging.isNull()) {
        UMLogger* logger;
        if (metricsLogging.isEmpty() || metricsLogging == "stdout") {
            logger = new UMFileLogger(stdout);
        } else if (metricsLogging == "lttng") {
            logger = new UMLTTNGLogger();
        } else {
            logger = new UMFileLogger(QString::fromLocal8Bit(metricsLogging));
        }
        if (logger->isOpen()) {
            applicationMonitor->installLogger(logger);
            applicationMonitor->setLogging(true);
        } else {
            delete logger;
        }
    }
    if (qEnvironmentVariableIsSet("UC_METRICS_OVERLAY")) {
        applicationMonitor->setOverlay(true);
    }

    // register performance monitor
    engine->rootContext()->setContextProperty(
        QStringLiteral("performanceMonitor"), new UCPerformanceMonitor(engine));
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
    qmlRegisterUncreatableType<QAbstractItemModel>(
        uri, 1, 1, "QAbstractItemModel", QStringLiteral("Not instantiable"));

    // register 1.1 only API
    qmlRegisterType<UCStyledItemBase, 1>(uri, 1, 1, "StyledItem");
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 1, 1, "SortFilterModel");
    qmlRegisterUncreatableType<FilterBehavior>(
        uri, 1, 1, "FilterBehavior", QStringLiteral("Not instantiable"));
    qmlRegisterUncreatableType<SortBehavior>(
        uri, 1, 1, "SortBehavior", QStringLiteral("Not instantiable"));
    qmlRegisterType<UCServiceProperties, 1>(uri, 1, 1, "ServiceProperties");

    // register 1.2 only API
    qmlRegisterType<UCListItem>(uri, 1, 2, "ListItem");
    qmlRegisterType<UCListItemDivider>();
    qmlRegisterUncreatableType<UCSwipeEvent>(
        uri, 1, 2, "SwipeEvent", QStringLiteral("This is an event object."));
    qmlRegisterUncreatableType<UCDragEvent>(
        uri, 1, 2, "ListItemDrag", QStringLiteral("This is an event object"));
    qmlRegisterType<UCListItemActions>(uri, 1, 2, "ListItemActions");
    qmlRegisterUncreatableType<UCViewItemsAttached>(
        uri, 1, 2, "ViewItems", QStringLiteral("Not instantiable"));
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
    qmlRegisterUncreatableType<UCSlotsAttached>(
        uri, 1, 3, "SlotsAttached", QStringLiteral("Not instantiable"));
    qmlRegisterUncreatableType<UCSlotsLayoutPadding>(
        uri, 1, 3, "SlotsLayoutPadding", QStringLiteral("Not instantiable"));
    qmlRegisterType<UCListItemLayout>(uri, 1, 3, "ListItemLayout");
    qmlRegisterType<UCHeader>(uri, 1, 3, "Header");
    qmlRegisterType<UCLabel>(uri, 1, 3, "Label");
    qmlRegisterType<UCBottomEdgeHint>(uri, 1, 3, "BottomEdgeHint");
    qmlRegisterType<UCBottomEdge>(uri, 1, 3, "BottomEdge");
    qmlRegisterType<UCBottomEdgeRegion>(uri, 1, 3, "BottomEdgeRegion");
    qmlRegisterType<UCPageTreeNode>(uri, 1, 3, "PageTreeNode");
    qmlRegisterType<UCPopupContext>(uri, 1, 3, "PopupContext");
    qmlRegisterType<UCMainViewBase>(uri, 1, 3, "MainViewBase");
    qmlRegisterType<ActionList>(uri, 1, 3, "ActionList");
    qmlRegisterType<ExclusiveGroup>(uri, 1, 3, "ExclusiveGroup");
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
    Q_UNUSED(plugin);

    // qmlplugindump requires that quickutils is initialized.
    QuickUtils::instance(engine);
}

void UbuntuLabsModule::defineModule(const char *uri)
{
    qmlRegisterType<SplitView>(uri, 1, 0, "SplitView");
    qmlRegisterType<SplitViewLayout>(uri, 1, 0, "SplitViewLayout");
    qmlRegisterType<ViewColumn>(uri, 1, 0, "ViewColumn");
    qmlRegisterType<Menu>(uri, 1, 0, "Menu");
    qmlRegisterType<MenuBar>(uri, 1, 0, "MenuBar");
    qmlRegisterType<MenuGroup>(uri, 1, 0, "MenuGroup");
}

void UbuntuLabsModule::undefineModule()
{
    // nothing yet
}

UT_NAMESPACE_END
