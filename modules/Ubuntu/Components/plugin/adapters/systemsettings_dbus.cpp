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

#include "systemsettings_p.h"
#include "systemsettings_p_p.h"
#include "systemsettings_dbus.h"
#include "dbuspropertywatcher_p.h"

// DBus adaptation
#include <QDebug>

SystemSettingsPrivate *createSystemSettingsAdaptation(SystemSettings *qq)
{
    return new SystemSettingsDBus(qq);
}

/******************************************************************************
 * DBus adaptation of the SystemSettingsPrivate
 */

#define ACCOUNTS_SERVICE    "org.freedesktop.Accounts"
#define ACCOUNTS_PATH       "/org/freedesktop/Accounts"
#define ACCOUNTS_IFACE      "org.freedesktop.Accounts"
#define VIBRA_PROPERTY      "OtherVibrate"

SystemSettingsDBus::SystemSettingsDBus(SystemSettings *qq)
    : SystemSettingsPrivate(qq)
    , accountsWatcher(QDBusConnection::systemBus(),
                      ACCOUNTS_SERVICE,
                      ACCOUNTS_PATH,
                      ACCOUNTS_IFACE,
                      (QStringList() << VIBRA_PROPERTY << "IncomingCallVibrate"))
{
}

// connect accounts watcher to get property changes
void SystemSettingsDBus::init()
{
    Q_Q(SystemSettings);
    QObject::connect(&accountsWatcher, SIGNAL(propertyChanged(QString,QVariant)),
                     q, SLOT(propertyChanged(QString,QVariant)));
    // get the value for each watched property
    accountsWatcher->fetchValues();
}

void SystemSettingsDBus::propertyChanged(const QString &property, const QVariant &value)
{
    qDebug() << property << value;
    if (property == VIBRA_PROPERTY) {
        vibrate = value.toBool();
        Q_Q(SystemSettings);
        Q_EMIT q->vibraEnabledChanged();
    }
}
