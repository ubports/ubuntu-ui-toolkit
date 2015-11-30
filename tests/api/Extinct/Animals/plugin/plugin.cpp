/*
 * Copyright 2014 Canonical Ltd.
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
#include "smilodon.h"

#include <QQuickItem>
#include <QtQml>

void ExtinctAnimalsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Extinct.Animals"));
    qmlRegisterType<UCArgument>(uri, 1, 0, "Smilodon");
    qmlRegisterType<UCArgument>(uri, 0, 1, "ScimitarCat");
    qmlRegisterType(QUrl(baseUrl().toString() + "/Andrewsarchus.qml"), uri, 0, 1, "Andrewsarchus");
    qmlRegisterType(QUrl(baseUrl().toString() + "/Paratriisodon.qml"), uri, 2, 0, "Andrewsarchus");
}
