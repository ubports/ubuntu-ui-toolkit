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
#include "ucalarm.h"
#include "ucalarm_p.h"

AlarmManagerPrivate::AlarmManagerPrivate(AlarmManager *qq)
    : q_ptr(qq)
    , alarmHolder(0)
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
}

AlarmManager::~AlarmManager()
{
}

AlarmManager &AlarmManager::instance()
{
    static AlarmManager instance;
    if (!instance.d_ptr->completed) {
        instance.d_ptr->init();
        instance.d_ptr->alarmHolder = new UCAlarm(&instance);
    }
    return instance;
}

bool AlarmManager::fetchAlarms()
{
    return d_ptr->fetchAlarms();
}

int AlarmManager::alarmCount()
{
    return d_ptr->alarmCount();
}

UCAlarm *AlarmManager::alarmAt(int index) const
{
    d_ptr->getAlarmAt(*d_ptr->alarmHolder, index);
    return d_ptr->alarmHolder;
}

UCAlarm *AlarmManager::findAlarm(const QVariant &cookie) const
{
    if (!d_ptr->findAlarm(*d_ptr->alarmHolder, cookie)) {
        return 0;
    };
    return d_ptr->alarmHolder;
}

bool AlarmManager::verifyChange(UCAlarm *alarm, Change change, const QVariant &newData)
{
    return d_ptr->verifyChange(alarm, change, newData);
}

UCAlarmPrivate *AlarmManager::createAlarmData(UCAlarm *alarm)
{
    return AlarmManagerPrivate::get()->createAlarmData(alarm);
}

#include "moc_alarmmanager_p.cpp"
