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

#include "dbuspropertywatcher_p.h"
#include <QtDBus/QDBusReply>
#include <unistd.h>
#include <sys/types.h>

/*
 * The class watches a dbus service on a given connection, path and interface for
 * different property changes. The properties parameter specifies the properties
 * to be watched. Multiple properties can be requested to be watched on the same
 * path and interface at the same time. If the property list is empty, the watcher
 * will report all the property changes on the path + interface.
 *
 * The property changes are reported thru the propertyChanged() signal.
 *
 * Example of use:
 * 1) watch all Accounts property changes
 * DBusPropertyWatcher watch(DBusConnection::systemBus(), "com.freedesktop.Accounts", "/com/freedesktop/Accounts",
 *                           "com.freedesktop.Accounts");
 *
 * 2) watch one property change
 * DBusPropertyWatcher watch(DBusConnection::systemBus(), "com.freedesktop.Accounts", "/com/freedesktop/Accounts",
 *                           "com.freedesktop.Accounts", QStringList("OtherVibrate"));
 *
 * 3) watch selected properties
 * DBusPropertyWatcher watch(DBusConnection::systemBus(), "com.freedesktop.Accounts", "/com/freedesktop/Accounts",
 *                           "com.freedesktop.Accounts",
 *                          (QStringList() << "IncomingCallVibrate" << "OtherVibrate"));
 */
DBusPropertyWatcher::DBusPropertyWatcher(const QDBusConnection &connection, const QString &service, const QString &path, const QString &iface, const QStringList &properties = QStringList(), QObject *parent)
    : QObject(parent)
    , connection(connection)
    , watcher(service, connection, QDBusServiceWatcher::WatchForOwnerChange)
    , iface(service, path, iface, connection)
    , service(service)
    , watchedProperties(properties)
{
    QObject::connect(&watcher, &QDBusServiceWatcher::serviceOwnerChanged,
                     this, &DBusPropertyWatcher::onOwnerChanged);
    setupInterface();
}

/*
 * Setup interface connections on ownership change
 */
void DBusPropertyWatcher::onOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    Q_UNUSED(newOwner);
    if (serviceName != service) {
        return;
    }
    setupInterface();
}

/*
 * Check the properties invalidated, and report the ones watched
 */
void DBusPropertyWatcher::onPropertiesChanged(const QString &interface, const QVariantMap &properties, const QStringList &invalidated)
{
    Q_UNUSED(properties);
    bool reportAll = watchedProperties.isEmpty();
    Q_FOREACH(const QString &property, invalidated) {
        if (reportAll || watchedProperties.contains(property)) {
            QVariant value = readProperty(interface, property);
            Q_EMIT propertyChanged(property, value);
        }
    }
}

/*
 * Connect dbus signal identified by (service, path, iface, name) quaduple to a slot to receive
 * property changes.
 */
void DBusPropertyWatcher::setupInterface()
{
    QDBusReply<QDBusObjectPath> qObjectPath = iface.call(
                "FindUserById", qlonglong(getuid()));

    if (qObjectPath.isValid()) {
        objectPath = qObjectPath.value().path();
        iface.connection().connect(
            service,
            objectPath,
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(onPropertiesChanged(QString,QVariantMap,QStringList)));
    }
}

/*
 * Read the property value synchronously.
 */
QVariant DBusPropertyWatcher::readProperty(const QString &interface, const QString &property)
{
    if (objectPath.isEmpty()) {
        // no path specified!
        return QVariant();
    }
    QDBusInterface readIFace(service, objectPath, "org.freedesktop.DBus.Properties", connection);
    if (!readIFace.isValid()) {
        // invalid interface
        return QVariant();
    }
    QDBusReply<QDBusVariant> reply = readIFace.call("Get", interface, property);
    return reply.isValid() ? reply.value().variant() : QVariant();
}
