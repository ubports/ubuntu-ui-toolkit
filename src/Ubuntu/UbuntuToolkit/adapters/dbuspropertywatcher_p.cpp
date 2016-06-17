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
#include "i18n.h"
#include <QtQml/QQmlInfo>

#define DYNAMIC_PROPERTY    "__q_property"

UT_NAMESPACE_BEGIN

UCServicePropertiesPrivate *createServicePropertiesAdapter(UCServiceProperties *owner)
{
    return new DBusServiceProperties(owner);
}

DBusServiceProperties::DBusServiceProperties(UCServiceProperties *qq)
    : UCServicePropertiesPrivate(qq)
    , connection("")
    , watcher(0)
    , iface(0)
{
}

bool DBusServiceProperties::init()
{
    // crear previous connections
    setStatus(UCServiceProperties::Inactive);
    delete iface;
    iface = 0;
    delete watcher;
    watcher = 0;
    setError(QString());

    if (service.isEmpty() || path.isEmpty()) {
        setStatus(UCServiceProperties::ConnectionError);
        setError("No service/path specified");
        return false;
    }

    switch (type) {
        case UCServiceProperties::System:
        {
            connection = QDBusConnection::systemBus();
            break;
        }
        case UCServiceProperties::Session:
        {
            connection = QDBusConnection::sessionBus();
            break;
        }
        default:
        {
            setStatus(UCServiceProperties::ConnectionError);
            setError(QStringLiteral("Invalid bus type: %1.").arg(type));
            return false;
        }
    }

    Q_Q(UCServiceProperties);
    // connect dbus watcher to catch OwnerChanged
    watcher = new QDBusServiceWatcher(service, connection, QDBusServiceWatcher::WatchForOwnerChange, q);
    // connect interface
    iface = new QDBusInterface(service, path, interface, connection, q);
    if (!iface->isValid()) {
        setStatus(UCServiceProperties::ConnectionError);
        setError(iface->lastError().message());
        return false;
    }
    // connect watcher to get owner changes
    QObject::connect(watcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                     this, SLOT(changeServiceOwner(QString,QString,QString)));
    return setupInterface();
}

/*
 * Connect dbus signal identified by (service, path, iface, name) quaduple to a
 * slot to receive property changes.
 */
bool DBusServiceProperties::setupInterface()
{
    QDBusReply<QDBusObjectPath> dbusObjectPath = iface->call("FindUserById", qlonglong(getuid()));
    if (dbusObjectPath.isValid()) {
        objectPath = dbusObjectPath.value().path();
        iface->connection().connect(
            service,
            objectPath,
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(updateProperties(QString,QVariantMap,QStringList)));
        return true;
    }

    setStatus(UCServiceProperties::ConnectionError);
    setError(dbusObjectPath.error().message());
    return false;
}

bool DBusServiceProperties::fetchPropertyValues()
{
    scannedProperties = properties;
    Q_FOREACH(QString property, properties) {
        readProperty(property);
    }
    return true;
}

/*
 * Reads a property value from the adaptorInterface asynchronously.
 */
bool DBusServiceProperties::readProperty(const QString &property)
{
    if ((status < UCServiceProperties::Synchronizing) || objectPath.isEmpty()) {
        return false;
    }
    Q_Q(UCServiceProperties);
    QDBusInterface readIFace(iface->interface(), objectPath, "org.freedesktop.DBus.Properties", connection);
    if (!readIFace.isValid()) {
        // report invalid interface only if the property's first letter was with capital one!
        if (property[0].isUpper()) {
            warning(readIFace.lastError().message());
        }
        return false;
    }
    QDBusPendingCall pending = readIFace.asyncCall("Get", adaptor, property);
    if (pending.isError()) {
        warning(pending.error().message());
        return false;
    }
    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(pending, q);
    QObject::connect(callWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(readFinished(QDBusPendingCallWatcher*)));

    // set a dynamic property so we know which property are we reading
    callWatcher->setProperty(DYNAMIC_PROPERTY, property);
    return true;
}

/*
 * Writes a property value to theadaptorInterface synchronously. It is for pure testing purposes.
 */
bool DBusServiceProperties::testProperty(const QString &property, const QVariant &value)
{
    if (objectPath.isEmpty()) {
        return false;
    }
    QDBusInterface writeIFace(iface->interface(), objectPath, "org.freedesktop.DBus.Properties", connection);
    if (!writeIFace.isValid()) {
        // invalid interface
        return false;
    }
    QDBusMessage msg = writeIFace.call("Set", adaptor, property, QVariant::fromValue(QDBusVariant(value)));
    return msg.type() == QDBusMessage::ReplyMessage;
}

/*
 * Slot called when the async read operation finishes.
 */
void DBusServiceProperties::readFinished(QDBusPendingCallWatcher *call)
{
    Q_Q(UCServiceProperties);
    QDBusPendingReply<QVariant> reply = *call;
    QString property = call->property(DYNAMIC_PROPERTY).toString();
    scannedProperties.removeAll(property);
    if (reply.isError()) {
        // remove the property from being watched, as it has no property like that
        properties.removeAll(property);
        if (property[0].isUpper()) {
            // report error!
            warning(reply.error().message());
        }
    } else {
        // update watched property value
        // make sure we have lower case when the property value is updated
        property[0] = property[0].toLower();
        q->setProperty(property.toLocal8Bit().constData(), reply.value());
    }

    if ((status == UCServiceProperties::Synchronizing) && scannedProperties.isEmpty()) {
        // set status to active
        setStatus(UCServiceProperties::Active);
    }

    // delete watcher
    call->deleteLater();
}

/*
 * Slot called when service owner is changed.
 */
void DBusServiceProperties::changeServiceOwner(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    Q_UNUSED(newOwner);
    if (serviceName != service) {
        return;
    }
    setupInterface();
}

/*
 * Slot called when the properties are changed in the service.
 */
void DBusServiceProperties::updateProperties(const QString &onInterface, const QVariantMap &map, const QStringList &invalidated)
{
    Q_UNUSED(onInterface);
    Q_UNUSED(map);
    Q_FOREACH(const QString &property, invalidated) {
        readProperty(property);
    }
}

UT_NAMESPACE_END
