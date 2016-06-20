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
 */

#include <QtQml/QQmlEngine>
#include "plugin.h"
#include "ucnamespace.h"
#include <ubuntutoolkitmodule.h>
#include <ubuntugesturesmodule.h>

UT_NAMESPACE_BEGIN

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));
    Q_UNUSED(uri);

    qmlRegisterSimpleSingletonType<UCNamespace>(uri, 1, 2, "Ubuntu");
    qmlRegisterSimpleSingletonType<UCNamespaceV13>(uri, 1, 3, "Ubuntu");

    UbuntuGesturesModule::defineModule(uri);
    UbuntuToolkitModule::defineModule();
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    UbuntuToolkitModule::initializeModule(engine, baseUrl());
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

UT_NAMESPACE_END
