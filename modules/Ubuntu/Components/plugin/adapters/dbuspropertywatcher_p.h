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

#include "ucserviceproperties_p.h"

class QDBusPendingCallWatcher;
class DBusServiceProperties : public QObject, public UCServicePropertiesPrivate
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(UCServiceProperties)
public:
    DBusServiceProperties(UCServiceProperties *qq);

    bool init();
    bool fetchPropertyValues();
    bool readProperty(const QString &property);
    // for testing purposes only!!!
    bool testProperty(const QString &property, const QVariant &value);

    QStringList scannedProperties;
    QDBusConnection connection;
    QDBusServiceWatcher *watcher;
    QDBusInterface *iface;
    QString objectPath;

    bool setupInterface();

public Q_SLOTS:
    void readFinished(QDBusPendingCallWatcher *watcher);
    void changeServiceOwner(const QString &serviceName, const QString &oldOwner, const QString &newOwner);
    void updateProperties(const QString &iface, const QVariantMap &map, const QStringList &invalidated);
};

#endif // DBUSPROPERTYWATCHER_P_H
