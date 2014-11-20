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

#include <QDebug>

// DBus adaptation

SystemSettingsPrivate *createSystemSettingsAdaptation()
{
    return new SystemSettingsDBus;
}

/******************************************************************************
 * DBus adaptation of the SystemSettingsPrivate
 */

SystemSettingsDBus::SystemSettingsDBus(QObject *parent)
    : QObject(parent)
{
}

SystemSettingsDBus::~SystemSettingsDBus()
{
}

void SystemSettingsDBus::updateVibraEnabled()
{

}

#include "moc_systemsettings_dbus.cpp"
