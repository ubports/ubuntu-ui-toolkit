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
#include "systemsettings_p_p.h"

class SystemSettingsDBus : public QObject, public SystemSettingsPrivate
{
    Q_OBJECT
public:
    SystemSettingsDBus(QObject *parent = 0);
    ~SystemSettingsDBus();

public Q_SLOTS:
    void updateVibraEnabled();
};

#endif // SYSTEMSETTINGS_DBUS_H
