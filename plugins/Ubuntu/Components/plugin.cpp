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

#include <QFont>
#include <QtDeclarative/QDeclarativeContext>

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    QDeclarativeExtensionPlugin::registerTypes(uri);
}

void UbuntuComponentsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
}

Q_EXPORT_PLUGIN2(UbuntuComponents, UbuntuComponentsPlugin)

