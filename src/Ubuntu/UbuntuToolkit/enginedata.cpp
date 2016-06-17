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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "enginedata.h"
#include <QtQml/QQmlEngine>

UT_NAMESPACE_BEGIN

const char *EngineProperty("__ubuntu_toolkit_plugin_data");

EngineData::EngineData(QObject *parent)
    : QObject(parent)
{
}

bool EngineData::isDeclared(QQmlEngine *engine)
{
    return engine && engine->property(EngineProperty).isValid();
}

void EngineData::create(QQmlEngine *engine, const QUrl &urlOverride)
{
    if (!engine) {
        return;
    }
    EngineData *data = new EngineData(engine);
    data->m_baseUrl = QUrl(urlOverride.toString() + '/');
    engine->setProperty(EngineProperty, QVariant::fromValue(data));
}

QUrl EngineData::baseUrl(QQmlEngine *engine)
{
    if (!engine) {
        return QUrl();
    }
    EngineData *data = engine->property(EngineProperty).value<EngineData*>();
    return data->m_baseUrl;
}

UT_NAMESPACE_END
