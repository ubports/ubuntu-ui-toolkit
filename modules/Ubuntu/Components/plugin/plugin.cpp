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

    qmlRegisterSingletonType<ThemeEngine>(uri, 0, 1, "Theme", ThemeEngine::initializeEngine);
    qmlRegisterType<StyleRule>(uri, 0, 1, "Rule");
    qmlRegisterType<StyledItem>(uri, 0, 1, "StyledItem");
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    // call engine registration method to load the theme
    ThemeEngine::initializeEngine(engine, 0);
}
