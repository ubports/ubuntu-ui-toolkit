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

#include "plugin.h"
#include "i18n.h"

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<UbuntuI18n>(uri, 0, 1, "I18n", "Singleton object");
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    engine->rootContext()->setContextProperty("i18n", &UbuntuI18n::instance());
    static ContextPropertyChangeListener i18nChangeListener(engine->rootContext(), "i18n");

//    engine->rootContext()->setContextProperty("units", &Units::instance());
//    static ContextPropertyChangeListener unitsChangeListener(engine->rootContext(), "units");
//    QObject::connect(&Units::instance(), SIGNAL(scaleFactorChanged()),
//                     &unitsChangeListener, SLOT(updateContextProperty()));
}
