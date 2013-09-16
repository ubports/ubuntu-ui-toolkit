/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include <QtQml>
#include <QtQuick/private/qquickimagebase_p.h>
#include <QDBusConnection>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "plugin.h"
#include "uctheme.h"

#include <QtQml/QQmlContext>
#include "i18n.h"
#include "listener.h"
#include "ucunits.h"
#include "ucscalingimageprovider.h"
#include "ucqquickimageextension.h"
#include "quickutils.h"
#include "giconprovider.h"
#include "shapeitem.h"
#include "inversemouseareatype.h"
#include "qquickclipboard.h"
#include "qquickmimedata.h"
#include "bottombarvisibilitycommunicator.h"
#include "ucubuntuanimation.h"
#include "ucfontutils.h"
#include "ucarguments.h"
#include "ucargument.h"
#include "ucapplication.h"
#include "ucalarm.h"
#include "ucalarmmodel.h"

#include <sys/types.h>
#include <unistd.h>

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


QUrl UbuntuComponentsPlugin::baseUrl(QStringList importPathList, const char* uri)
{
    /* FIXME: remove when migrating to Qt 5.1 and use QQmlExtensionPlugin::baseUrl()
       http://doc-snapshot.qt-project.org/qt5-stable/qtqml/qqmlextensionplugin.html#baseUrl
    */
    QString pluginRelativePath = QString::fromUtf8(uri).replace(".", "/").prepend("/").append("/");
    QString pluginPath;
    Q_FOREACH (QString importPath, importPathList) {
        pluginPath = importPath.append(pluginRelativePath);
        /* We verify that the directory ending in Ubuntu/Components contains the
           file libUbuntuComponents.so therefore proving it's the right directory.

           Ref.: https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1197293
        */
        if (QFile(pluginPath + "libUbuntuComponents.so").exists()) {
            return QUrl::fromLocalFile(pluginPath);
        }
    }

    return QUrl();
}

void UbuntuComponentsPlugin::registerQmlSingletonType(QQmlEngine *engine, const char* uri, const char* typeName, const char* qmlFile)
{
    QUrl url = baseUrl(engine->importPathList(), uri).resolved(QUrl::fromLocalFile(qmlFile));
    QObject* object = QuickUtils::instance().createQmlObject(url);
    if (object != NULL) {
        engine->rootContext()->setContextProperty(typeName, object);
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
                     Qt::DirectConnection);

}

void UbuntuComponentsPlugin::setWindowContextProperty(QWindow* focusWindow)
{
    QQuickView* view = qobject_cast<QQuickView*>(focusWindow);

    if (view != NULL) {
        view->rootContext()->setContextProperty("window", view);
    }
}

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));

    qmlRegisterUncreatableType<UbuntuI18n>(uri, 0, 1, "i18n", "Singleton object");
    qmlRegisterExtendedType<QQuickImageBase, UCQQuickImageExtension>(uri, 0, 1, "QQuickImageBase");
    qmlRegisterUncreatableType<UCUnits>(uri, 0, 1, "UCUnits", "Not instantiable");
    qmlRegisterType<ShapeItem>(uri, 0, 1, "Shape");
    qmlRegisterType<InverseMouseAreaType>(uri, 0, 1, "InverseMouseArea");
    qmlRegisterType<QQuickMimeData>(uri, 0, 1, "MimeData");
    qmlRegisterSingletonType<QQuickClipboard>(uri, 0, 1, "Clipboard", registerClipboard);
    qmlRegisterSingletonType<UCUbuntuAnimation>(uri, 0, 1, "UbuntuAnimation", registerUCUbuntuAnimation);
    qmlRegisterType<UCArguments>(uri, 0, 1, "Arguments");
    qmlRegisterType<UCArgument>(uri, 0, 1, "Argument");
    qmlRegisterType<QQmlPropertyMap>();
    qmlRegisterType<UCAlarm>(uri, 0, 1, "Alarm");
    qmlRegisterType<UCAlarmModel>(uri, 0, 1, "AlarmModel");
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    QQmlContext* context = engine->rootContext();

    QuickUtils::instance().setImportPathList(engine->importPathList());

    // register root object watcher that sets a global property with the root object
    // that can be accessed from any object
    context->setContextProperty("QuickUtils", &QuickUtils::instance());

    UCTheme::instance().registerToContext(context);

    context->setContextProperty("i18n", &UbuntuI18n::instance());
    static ContextPropertyChangeListener i18nChangeListener(context, "i18n");
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(domainChanged()),
                     &i18nChangeListener, SLOT(updateContextProperty()));
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(languageChanged()),
                     &i18nChangeListener, SLOT(updateContextProperty()));

    // We can't use 'Application' because it exists (undocumented)
    context->setContextProperty("UbuntuApplication", &UCApplication::instance());
    static ContextPropertyChangeListener applicationChangeListener(context, "UbuntuApplication");
    QObject::connect(&UCApplication::instance(), SIGNAL(applicationNameChanged()),
                     &applicationChangeListener, SLOT(updateContextProperty()));

    context->setContextProperty("units", &UCUnits::instance());
    static ContextPropertyChangeListener unitsChangeListener(context, "units");
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     &unitsChangeListener, SLOT(updateContextProperty()));

    // register FontUtils
    context->setContextProperty("FontUtils", &UCFontUtils::instance());
    static ContextPropertyChangeListener fontUtilsListener(context, "FontUtils");
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     &fontUtilsListener, SLOT(updateContextProperty()));

    context->setContextProperty("bottomBarVisibilityCommunicator", &BottomBarVisibilityCommunicator::instance());

    // register UbuntuColors
    registerQmlSingletonType(engine, uri, "UbuntuColors", "Colors/UbuntuColors.qml");

    engine->addImageProvider(QLatin1String("scaling"), new UCScalingImageProvider);

    // register gicon provider
    engine->addImageProvider(QLatin1String("gicon"), new GIconProvider);

    // Necessary for Screen.orientation (from import QtQuick.Window 2.0) to work
    QGuiApplication::primaryScreen()->setOrientationUpdateMask(
            Qt::PortraitOrientation |
            Qt::LandscapeOrientation |
            Qt::InvertedPortraitOrientation |
            Qt::InvertedLandscapeOrientation);

    registerWindowContextProperty();
}
