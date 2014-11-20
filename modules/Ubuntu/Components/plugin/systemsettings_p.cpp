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

SystemSettingsPrivate::SystemSettingsPrivate()
    : q_ptr(0)
    , vibrate(false)
{
}

SystemSettingsPrivate::~SystemSettingsPrivate()
{
}

void SystemSettingsPrivate::init(SystemSettings *qq)
{
    q_ptr = qq;
}

/*
 * The class maintains the access to some common system settings required by the
 * toolkit. The private class serves the adaptation interface.
 */
SystemSettings::SystemSettings(QObject *parent)
    : QObject(parent)
    , d_ptr(createSystemSettingsAdaptation())
{
    d_ptr->init(this);
}

SystemSettings::~SystemSettings()
{
}

bool SystemSettings::vibraEnabled() const
{
    return d_ptr->vibrate;
}
