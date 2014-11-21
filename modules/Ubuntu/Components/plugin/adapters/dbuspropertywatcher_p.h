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

#ifndef DBUSPROPERTYWATCHER_P_H
#define DBUSPROPERTYWATCHER_P_H

#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusInterface>

class DBusPropertyWatcher : public QObject
{
    Q_OBJECT
public:
    explicit DBusPropertyWatcher(const QDBusConnection &connection, const QString &service, const QString &path, const QString &iface, const QStringList &properties, QObject *parent = 0);

Q_SIGNALS:
    void propertyChanged(const QString &property, const QVariant &value);

private Q_SLOTS:
    void onOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner);
    void onPropertiesChanged(const QString &iface, const QVariantMap &properties, const QStringList &invalidated);

private:
    QDBusConnection connection;
    QDBusServiceWatcher watcher;
    QDBusInterface iface;
    QString service;
    QString objectPath;
    QStringList watchedProperties;

    void setupInterface();
    QVariant readProperty(const QString &interface, const QString &property);
};

#endif // DBUSPROPERTYWATCHER_P_H
