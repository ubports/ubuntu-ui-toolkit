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
#include "themeengine.h"
#include "itemstyleattached.h"

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
#include "ucstyle.h"
#include "ucubuntuanimation.h"
#include "ucfontutils.h"

#include <sys/types.h>
#include <unistd.h>

/*
 * Registration function for the Clipboard type
 */
static QObject *registerClipboard(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    QQuickClipboard *clipboard = new QQuickClipboard;
    return clipboard;
}

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));

    qmlRegisterUncreatableType<ItemStyleAttached>(uri, 0, 1, "ItemStyle", "Type is not instantiable.");
    qmlRegisterUncreatableType<UbuntuI18n>(uri, 0, 1, "i18n", "Singleton object");
    qmlRegisterExtendedType<QQuickImageBase, UCQQuickImageExtension>(uri, 0, 1, "QQuickImageBase");
    qmlRegisterUncreatableType<UCUnits>(uri, 0, 1, "UCUnits", "Not instantiable");
    qmlRegisterType<ShapeItem>(uri, 0, 1, "Shape");
    qmlRegisterType<InverseMouseAreaType>(uri, 0, 1, "InverseMouseArea");
    qmlRegisterType<QQuickMimeData>(uri, 0, 1, "MimeData");
    qmlRegisterSingletonType<QQuickClipboard>(uri, 0, 1, "Clipboard", registerClipboard);
    qmlRegisterType<UCStyle>(uri, 0, 1, "Style");
    qmlRegisterSingletonType<UCUbuntuAnimation>(uri, 0, 1, "UbuntuAnimation", registerUCUbuntuAnimation);
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    // call engine registration method to load the theme
    QQmlContext* context = engine->rootContext();

    // register root object watcher that sets a global property with the root object
    // that can be accessed from any object
    context->setContextProperty("QuickUtils", &QuickUtils::instance());

    context->setContextProperty("Theme", ThemeEngine::initializeEngine(engine));
    context->setContextProperty("i18n", &UbuntuI18n::instance());
    static ContextPropertyChangeListener i18nChangeListener(context, "i18n");
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(domainChanged()),
                     &i18nChangeListener, SLOT(updateContextProperty()));
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(languageChanged()),
                     &i18nChangeListener, SLOT(updateContextProperty()));

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

    engine->addImageProvider(QLatin1String("scaling"), new UCScalingImageProvider);

    // register gicon provider
    engine->addImageProvider(QLatin1String("gicon"), new GIconProvider);

    // Necessary for Screen.orientation (from import QtQuick.Window 2.0) to work
    QGuiApplication::primaryScreen()->setOrientationUpdateMask(
            Qt::PortraitOrientation |
            Qt::LandscapeOrientation |
            Qt::InvertedPortraitOrientation |
            Qt::InvertedLandscapeOrientation);
}
