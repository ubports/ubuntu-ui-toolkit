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

#ifndef SYSTEMSETTINGS_DBUS_H
#define SYSTEMSETTINGS_DBUS_H

#include <QtCore/QObject>
//#include "systemsettings_p.h"
#include "systemsettings_p_p.h"
#include "dbuspropertywatcher_p.h"

class SystemSettingsDBus : public SystemSettingsPrivate
{
    Q_DECLARE_PUBLIC(SystemSettings)
public:
    SystemSettingsDBus(SystemSettings *qq);

    // interface
    void init();
    void propertyChanged(const QString &property, const QVariant &value);
    bool hasProperty(const QString &property);
    bool testProperty(const QString &property, const QVariant &value);

private:
    DBusPropertyWatcher accountsWatcher;
};

#endif // SYSTEMSETTINGS_DBUS_H
