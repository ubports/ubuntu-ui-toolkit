/*
 * Copyright 2013 Canonical Ltd.
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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "alarmmanager_p.h"
#include "alarmmanager_p_p.h"
#include "alarmrequest_p.h"
#include "alarmrequest_p_p.h"
#include "ucalarm.h"
#include "ucalarm_p.h"

AlarmManagerPrivate::AlarmManagerPrivate(AlarmManager *qq)
    : q_ptr(qq)
    , completed(false)
{
}

AlarmManagerPrivate::~AlarmManagerPrivate()
{
}

/*-----------------------------------------------------------------------------
 * Alarm Manager public interface
 */
AlarmManager::AlarmManager(QObject *parent)
    : QObject(parent)
    , d_ptr(createAlarmsAdapter(this))
{
    d_ptr->fetchAlarms();
}

AlarmManager::~AlarmManager()
{
}

QList<AlarmData> AlarmManager::alarms() const
{
    Q_D(const AlarmManager);
    return d->alarmList;
}

#include "moc_alarmmanager_p.cpp"
