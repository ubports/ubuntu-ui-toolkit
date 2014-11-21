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

#include <QDebug>
// DBus adaptation

SystemSettingsPrivate *createSystemSettingsAdaptation()
{
    return new SystemSettingsDBus;
}

/******************************************************************************
 * DBus adaptation of the SystemSettingsPrivate
 */

#define ACCOUNTS_SERVICE    "org.freedesktop.Accounts"
#define ACCOUNTS_PATH       "/org/freedesktop/Accounts"
#define ACCOUNTS_IFACE      "org.freedesktop.Accounts"

SystemSettingsDBus::SystemSettingsDBus(QObject *parent)
    : QObject(parent)
    , accountsWatcher(QDBusConnection::systemBus(),
                      "org.freedesktop.Accounts",
                      "/org/freedesktop/Accounts",
                      "org.freedesktop.Accounts",
                      QStringList("OtherVibrate"))
{
}

void SystemSettingsDBus::init(SystemSettings *qq)
{
    SystemSettingsPrivate::init(qq);
    connect(&accountsWatcher, &DBusPropertyWatcher::propertyChanged,
            this, &SystemSettingsDBus::vibrateChanged);
}

void SystemSettingsDBus::vibrateChanged(const QString &property, const QVariant &value)
{
//    Q_UNUSED(property);
    qDebug() << property;
    if (property == "OtherVibrate") {
    vibrate = value.toBool();
    Q_EMIT q_ptr->vibraEnabledChanged();
    }
}

#include "moc_systemsettings_dbus.cpp"
