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

#include "plugin.h"

#include <QtDeclarative/QDeclarativeExtensionPlugin>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeItem>

#include "i18n.h"

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
//    Q_UNUSED(uri);
//    qmlRegisterType<UbuntuI18n>(uri, 0, 1, "I18n"); // @uri Ubuntu.Components
//    qmlRegisterType<UbuntuI18n>("Ubuntu.Components", 0, 1, "Tim"); // @uri Ubuntu.Components
//    qmlRegisterUncreateableType<UbuntuI18n>(uri, 0, 1, "I18n");
    qmlRegisterUncreatableType<UbuntuI18n>(uri, 0, 1, "I18n", "whatever"); // TODO: give reason
}

void UbuntuComponentsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
    UbuntuI18n* i = new UbuntuI18n();
    i->init("ubuntu-sdk", "/usr/share/locale");
//    engine->rootContext()->setContextProperty("i18n", new UbuntuI18n());
    engine->rootContext()->setContextProperty("i18n", i);
}

Q_EXPORT_PLUGIN2(UbuntuComponents, UbuntuComponentsPlugin)

