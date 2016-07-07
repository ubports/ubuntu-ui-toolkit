/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

#include "ucurihandler.h"
#include <QtDBus/QtDBus>
#include <libnih.h>
#include <libnih-dbus.h>
#include "statesaverbackend_p.h"

UT_NAMESPACE_BEGIN

UriHandlerObject::UriHandlerObject(UCUriHandler* uriHandler)
    : m_uriHandler(uriHandler)
{
}

void UriHandlerObject::Open(const QStringList& uris, const QHash<QString, QVariant>& platformData)
{
    Q_UNUSED(platformData);
    Q_EMIT m_uriHandler->opened(uris);
}

/*!
 * \qmltype UriHandler
 * \instantiates UbuntuToolkit::UCUriHandler
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief Singleton signalling for opened URIs.
 *
 * UriHandler is a singleton handling opened URIs. The application can be signalled of opened URIs
 * through the opened signal. The content of the "APP_ID" environment variable is used to determine
 * the object path. If the D-Bus session bus is not connected or the "APP_ID" variable is not set or
 * empty, the handler stays uninitialized.
 *
 * Example of use:
 *
 * \qml
 * Connections {
 *     target: UriHandler
 *     onOpened: print(uris)
 * }
 * \endqml
 */
UCUriHandler::UCUriHandler()
    : m_uriHandlerObject(this)
{
    QString objectPath;

    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "UCUriHandler: D-Bus session bus is not connected, ignoring.";
        return;
    }

    // Get the object path based on the "APP_ID" environment variable.
    QByteArray applicationId = qgetenv("APP_ID");
    if (applicationId.isEmpty()) {
        qWarning() << "UCUriHandler: Empty \"APP_ID\" environment variable, ignoring.";
        return;
    }
    char* path = nih_dbus_path(NULL, "", applicationId.constData(), NULL);
    objectPath = QString(path);
    nih_free(path);

    // Ensure handler is running on the main thread.
    QCoreApplication* instance = QCoreApplication::instance();
    if (instance) {
        moveToThread(instance->thread());
    } else {
        qWarning() << "UCUriHandler: Created before QCoreApplication, application may misbehave.";
    }

    QDBusConnection::sessionBus().registerObject(
        objectPath, &m_uriHandlerObject, QDBusConnection::ExportAllSlots);
}

/*!
 * \qmlsignal UriHandler::onOpened(list<string> uris)
 *
 * The signal is triggered when URIs are opened.
 */

UT_NAMESPACE_END
