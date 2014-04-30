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
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "upmplugin.h"
#include "upmcpuusage.h"
#include "upmtexturefromimage.h"
#include "upmgraphmodel.h"

#include <qqml.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include "upmrenderingtimes.h"

void UbuntuPerformanceMetricsPlugin::registerTypeVersion(const char *uri, int major, int minor)
{
    qmlRegisterType<UPMRenderingTimes>(uri, major, minor, "RenderingTimes");
    qmlRegisterType<UPMCpuUsage>(uri, major, minor, "CpuUsage");
    qmlRegisterType<UPMTextureFromImage>(uri, major, minor, "TextureFromImage");
    qmlRegisterType<UPMGraphModel>();
}

void UbuntuPerformanceMetricsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.PerformanceMetrics"));
    registerTypeVersion(uri, 0, 1);
    registerTypeVersion(uri, 1, 0);
}

void UbuntuPerformanceMetricsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    QQmlContext* context = engine->rootContext();
    QByteArray performanceOverlay = qgetenv("PERFORMANCE_OVERLAY");
    context->setContextProperty("performanceOverlayEnabled", QVariant(!performanceOverlay.isEmpty()));
}
