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

// FIXME(loicm): Documentation isn't built correctly.

#include "ucurihandler.h"
#include <QtDBus/QtDBus>
// FIXME(loicm) Including <libnih-dbus.h> generates C++ compilation errors.
#include <libnih.h>
#include <libnih-dbus.h>
//#include <nih-dbus/dbus_util.h>

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
 * \qmltype UCUriHandler
 * \instantiates UCUriHandler
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief Singleton signalling for opened URIs.
 *
 * UriHandler is a singleton handling opened URIs. The application can be signalled of opened URIs
 * through the \l opened signal. The content of the "APP_ID" environment variable is used to
 * determine the object path. If the D-Bus session bus is not connected or the "APP_ID" variable is
 * not set or empty, the handler stays uninitialized.
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
    m_objectPath = QString(path);
    nih_free(path);
    Q_EMIT objectPathChanged();

    // Ensure handler is running on the main thread.
    QCoreApplication* instance = QCoreApplication::instance();
    if (instance) {
        moveToThread(instance->thread());
    } else {
        qWarning() << "UCUriHandler: Created before QCoreApplication, application may misbehave.";
    }

    QDBusConnection::sessionBus().registerObject(
        m_objectPath, &m_uriHandlerObject, QDBusConnection::ExportAllSlots);
    QDBusConnection::sessionBus().registerService("org.freedesktop.Application");
}

/*!
 * \qmlproperty string UriHandler::objectPath
 *
 * This property corresponds to the D-Bus object path used by the UriHandler once initialized
 * successfully.
 */

/*!
 * \qmlsignal UriHandler::opened(string uris[])
 *
 * The signal is triggered when URIs are opened.
 */
