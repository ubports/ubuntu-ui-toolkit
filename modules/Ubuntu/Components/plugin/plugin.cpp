/*
 * Copyright 2015 Canonical Ltd.
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
 */

#include <QtQuick/private/qquickimagebase_p.h>
#include <QDBusConnection>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "plugin.h"
#include "uctheme.h"
#include "ucdeprecatedtheme.h"

#include <QtQml/QQmlContext>
#include "i18n.h"
#include "listener.h"
#include "ucunits.h"
#include "ucscalingimageprovider.h"
#include "ucqquickimageextension.h"
#include "quickutils.h"
#include "ucubuntushape.h"
#include "ucubuntushapeoverlay.h"
#include "inversemouseareatype.h"
#include "qquickclipboard.h"
#include "qquickmimedata.h"
#include "ucubuntuanimation.h"
#include "ucfontutils.h"
#include "ucarguments.h"
#include "ucargument.h"
#include "ucapplication.h"
#include "ucalarm.h"
#include "ucalarmmodel.h"
#include "unitythemeiconprovider.h"
#include "ucstatesaver.h"
#include "ucurihandler.h"
#include "ucmouse.h"
#include "ucinversemouse.h"
#include "sortfiltermodel.h"
#include "ucstyleditembase.h"
#include "ucstylehints.h"
#include "ucaction.h"
#include "ucactioncontext.h"
#include "ucactionmanager.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemactions.h"
#include "uclistitemstyle.h"
#include "ucserviceproperties.h"
#include "ucnamespace.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>

QUrl UbuntuComponentsPlugin::m_baseUrl = QUrl();

/*
 * Type registration functions.
 */

static QObject *registerClipboard(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    QQuickClipboard *clipboard = new QQuickClipboard;
    return clipboard;
}

static QObject *registerUCUbuntuAnimation(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    UCUbuntuAnimation *animation = new UCUbuntuAnimation();
    return animation;
}

static QObject *registerUriHandler(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    UCUriHandler *uriHandler = new UCUriHandler();
    return uriHandler;
}

static QObject *registerUbuntuNamespace(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new UCNamespace();
}

static QObject *registerUbuntuNamespace13(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new UCNamespaceV13();
}

void UbuntuComponentsPlugin::initializeBaseUrl()
{
    if (!m_baseUrl.isValid()) {
        m_baseUrl = QUrl(baseUrl().toString() + '/');
    }
}

void UbuntuComponentsPlugin::registerWindowContextProperty()
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

void UbuntuComponentsPlugin::setWindowContextProperty(QWindow* focusWindow)
{
    QQuickView* view = qobject_cast<QQuickView*>(focusWindow);

    if (view != NULL) {
        view->rootContext()->setContextProperty("window", view);
    }
}

void UbuntuComponentsPlugin::registerTypesToVersion(const char *uri, int major, int minor)
{
    qmlRegisterType<UCAction>(uri, major, minor, "Action");
    qmlRegisterType<UCActionContext>(uri, major, minor, "ActionContext");
    qmlRegisterType<UCActionManager>(uri, major, minor, "ActionManager");
    qmlRegisterType<UCStyledItemBase>(uri, major, minor, "StyledItem");
    qmlRegisterUncreatableType<UbuntuI18n>(uri, major, minor, "i18n", "Singleton object");
    qmlRegisterExtendedType<QQuickImageBase, UCQQuickImageExtension>(uri, major, minor, "QQuickImageBase");
    qmlRegisterUncreatableType<UCUnits>(uri, major, minor, "UCUnits", "Not instantiable");
    qmlRegisterType<UCUbuntuShape>(uri, major, minor, "UbuntuShape");
    // FIXME/DEPRECATED: Shape is exported for backwards compatibility only
    qmlRegisterType<UCUbuntuShape>(uri, major, minor, "Shape");
    qmlRegisterType<InverseMouseAreaType>(uri, major, minor, "InverseMouseArea");
    qmlRegisterType<QQuickMimeData>(uri, major, minor, "MimeData");
    qmlRegisterSingletonType<QQuickClipboard>(uri, major, minor, "Clipboard", registerClipboard);
    qmlRegisterSingletonType<UCUbuntuAnimation>(uri, major, minor, "UbuntuAnimation", registerUCUbuntuAnimation);
    qmlRegisterType<UCArguments>(uri, major, minor, "Arguments");
    qmlRegisterType<UCArgument>(uri, major, minor, "Argument");
    qmlRegisterType<QQmlPropertyMap>();
    qmlRegisterType<UCAlarm>(uri, major, minor, "Alarm");
    qmlRegisterType<UCAlarmModel>(uri, major, minor, "AlarmModel");
    qmlRegisterType<UCStateSaver>(uri, major, minor, "StateSaver");
    qmlRegisterType<UCStateSaverAttached>();
    qmlRegisterSingletonType<UCUriHandler>(uri, major, minor, "UriHandler", registerUriHandler);
    qmlRegisterType<UCMouse>(uri, major, minor, "Mouse");
    qmlRegisterType<UCInverseMouse>(uri, major, minor, "InverseMouse");
}

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));
    initializeBaseUrl();

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
    qmlRegisterSingletonType<UCNamespace>(uri, 1, 2, "Ubuntu", registerUbuntuNamespace);
    qmlRegisterType<UCUbuntuShape, 1>(uri, 1, 2, "UbuntuShape");
    qmlRegisterType<UCUbuntuShapeOverlay>(uri, 1, 2, "UbuntuShapeOverlay");

    // register 1.3 API
    qmlRegisterType<UCTheme>(uri, 1, 3, "ThemeSettings");
    qmlRegisterType<UCStyledItemBase, 2>(uri, 1, 3, "StyledItem");
    qmlRegisterSingletonType<UCNamespaceV13>(uri, 1, 3, "Ubuntu", registerUbuntuNamespace13);
    qmlRegisterType<UCStyledItemBase, 2>(uri, 1, 3, "StyledItem");
    qmlRegisterCustomType<UCStyleHints>(uri, 1, 3, "StyleHints", new UCStyleHintsParser);
    qmlRegisterType<UCAction, 1>(uri, 1, 3, "Action");
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    // initialize baseURL
    initializeBaseUrl();

    // register internal styles
    const char *styleUri = "Ubuntu.Components.Styles";
    qmlRegisterType<UCListItemStyle>(styleUri, 1, 2, "ListItemStyle");
    qmlRegisterType<UCListItemStyle, 1>(styleUri, 1, 3, "ListItemStyle");

    QQmlExtensionPlugin::initializeEngine(engine, uri);
    QQmlContext* context = engine->rootContext();

    // register root object watcher that sets a global property with the root object
    // that can be accessed from any object
    context->setContextProperty("QuickUtils", &QuickUtils::instance());

    // register theme context property
    UCTheme::registerToContext(context);

    UCDeprecatedTheme::instance().registerToContext(context);

    context->setContextProperty("i18n", &UbuntuI18n::instance());
    ContextPropertyChangeListener *i18nChangeListener =
        new ContextPropertyChangeListener(context, "i18n");
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(domainChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(languageChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));

    // We can't use 'Application' because it exists (undocumented)
    context->setContextProperty("UbuntuApplication", &UCApplication::instance());
    ContextPropertyChangeListener *applicationChangeListener =
        new ContextPropertyChangeListener(context, "UbuntuApplication");
    QObject::connect(&UCApplication::instance(), SIGNAL(applicationNameChanged()),
                     applicationChangeListener, SLOT(updateContextProperty()));
    // Give the application object access to the engine
    UCApplication::instance().setContext(context);

    context->setContextProperty("units", &UCUnits::instance());
    ContextPropertyChangeListener *unitsChangeListener =
        new ContextPropertyChangeListener(context, "units");
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     unitsChangeListener, SLOT(updateContextProperty()));

    // register FontUtils
    context->setContextProperty("FontUtils", &UCFontUtils::instance());
    ContextPropertyChangeListener *fontUtilsListener =
        new ContextPropertyChangeListener(context, "FontUtils");
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     fontUtilsListener, SLOT(updateContextProperty()));

    engine->addImageProvider(QLatin1String("scaling"), new UCScalingImageProvider);

    // register icon provider
    engine->addImageProvider(QLatin1String("theme"), new UnityThemeIconProvider);

    // Necessary for Screen.orientation (from import QtQuick.Window 2.0) to work
    QGuiApplication::primaryScreen()->setOrientationUpdateMask(
            Qt::PortraitOrientation |
            Qt::LandscapeOrientation |
            Qt::InvertedPortraitOrientation |
            Qt::InvertedLandscapeOrientation);

    registerWindowContextProperty();
}
