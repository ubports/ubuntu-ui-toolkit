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

#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>

#include "style.h"
#include "themeengine.h"
#include "styleditem.h"

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));
    qmlRegisterType<StyleRule>(uri, 0, 1, "Rule");

    // these need to be registered to QtQuick 2.0 otherwise the revisioned
    // properties won't be accessible
    const char *quick = "QtQuick";
    qmlRegisterType<StyledItem, 1>(quick, 2, 0, "StyledItem");
    qmlRegisterRevision<QQuickItem, 1>(quick, 2, 0);
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    ThemeEngine::initialize(engine);
    // add theme engine
    engine->rootContext()->setContextProperty("theme", ThemeEngine::instance());
}
