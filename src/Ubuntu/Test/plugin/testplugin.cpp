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
 */

#include "testplugin.h"
#include <QtQml>
#include "uctestextras.h"
#include <UbuntuToolkit/private/mousetouchadaptor_p.h>

UT_USE_NAMESPACE

static QObject *registerExtras(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new UCTestExtras;
}

void TestPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<UCTestExtras>(uri, 1, 0, "TestExtras", registerExtras);
    qmlRegisterSingletonType<MouseTouchAdaptor>(uri, 1, 0, "MouseTouchAdaptor", MouseTouchAdaptor::registerQmlSingleton);
}
