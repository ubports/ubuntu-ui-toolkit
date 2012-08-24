/*
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
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
 */

#include "plugin.h"

#include <QFont>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativePropertyMap>
#include <QtDeclarative/QDeclarativeContext>


void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri);

    // @uri Ubuntu.Components
    //qmlRegisterType<SomeCppClass>(uri, 0, 1, "SomeCppClass");
}

void UbuntuComponentsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);

    engine->rootContext()->setContextProperty("UiConstants", uiConstants());
}

QDeclarativePropertyMap* UbuntuComponentsPlugin::uiConstants() {

    QString defaultFontFamily      = QLatin1String("Ubuntu Sans");
    QString defaultFontFamilyLight = QLatin1String("Ubuntu Sans Light");

    QDeclarativePropertyMap *uiConstantsData = new QDeclarativePropertyMap();

    QFont bodyFont;
    bodyFont.setFamily(defaultFontFamily);
    bodyFont.setPointSize(12);
    uiConstantsData->insert("BodyTextFont", QVariant(bodyFont));

    QFont headerFont;
    headerFont.setFamily(defaultFontFamilyLight);
    headerFont.setPointSize(16);
    uiConstantsData->insert("HeaderFont", QVariant(headerFont));

    return uiConstantsData;
}

Q_EXPORT_PLUGIN2(UbuntuComponents, UbuntuComponentsPlugin)

