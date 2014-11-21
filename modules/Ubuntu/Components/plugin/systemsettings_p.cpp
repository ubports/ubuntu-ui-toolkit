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

SystemSettingsPrivate::SystemSettingsPrivate(SystemSettings *qq)
    : q_ptr(qq)
    , vibrate(true)
{
}

SystemSettingsPrivate::~SystemSettingsPrivate()
{
}

SystemSettingsPrivate *SystemSettingsPrivate::get(SystemSettings *settings)
{
    return settings->d_ptr;
}


/*
 * The class maintains the access to some common system settings required by the
 * toolkit. The private class serves the adaptation interface.
 */
SystemSettings::SystemSettings(QObject *parent)
    : QObject(parent)
    , d_ptr(createSystemSettingsAdaptation(this))
{
    Q_D(SystemSettings);
    d->init();
}

SystemSettings::~SystemSettings()
{
}

bool SystemSettings::vibraEnabled() const
{
    Q_D(const SystemSettings);
    return d->vibrate;
}
