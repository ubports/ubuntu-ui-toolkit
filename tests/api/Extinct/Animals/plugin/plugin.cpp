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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include "plugin.h"
#include "tarpan.h"
#include "smilodon.h"
#include "pangaea.h"
#include "rune.h"
#include "europe.h"

#include <QtQuick/private/qquickimagebase_p.h>

static QObject *registerPangaea(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new EAPangaea();
}

static QObject *registerPangaea4(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new EAPangaea4();
}

void ExtinctAnimalsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    Q_UNUSED(engine);
}

void ExtinctAnimalsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Extinct.Animals"));

    qmlRegisterType<EATarpan>();
    qmlRegisterType<EASmilodon>(uri, 1, 0, "Smilodon");
    qmlRegisterType<EASmilodon>(uri, 0, 1, "ScimitarCat");

    qmlRegisterType(QUrl(baseUrl().toString() + "/Andrewsarchus.qml"), uri, 0, 1, "Andrewsarchus");
    // The second call overrrides the previous one, only the last version will show up
    qmlRegisterType(QUrl(baseUrl().toString() + "/Paratriisodon.qml"), uri, 2, 0, "Andrewsarchus");

    qmlRegisterType(QUrl("qrc:///ea/Paratriisodon.qml"), uri, 2, 3, "AndrewsarchusMongoliensis");
    // The second call overrrides the previous one, only the last version will show up
    qmlRegisterType(QUrl("qrc:///ea/Paratriisodon.qml"), uri, 2, 4, "AndrewsarchusMongoliensis");

    qmlRegisterSingletonType<EAPangaea>(uri, 1, 0, "Pangaea", registerPangaea);
    qmlRegisterSingletonType<EAPangaea4>(uri, 4, 0, "Pangaea", registerPangaea4);

    qmlRegisterUncreatableType<EAEurope>(uri, 4, 1, "Europe", "");
    qmlRegisterUncreatableType<EAEurope42, 1>(uri, 4, 2, "Europe", "");

    qmlRegisterExtendedType<QQuickImageBase, UCQQuickImageExtension>(uri, 3, 3, "QQuickImageBase");
}
