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
    if (!instance.d_func()->completed) {
        instance.d_func()->init();
    }
    return instance;
}

bool AlarmManager::fetchAlarms()
{
    Q_D(AlarmManager);
    return d->fetchAlarms();
}

int AlarmManager::alarmCount()
{
    Q_D(AlarmManager);
    return d->alarmCount();
}

UCAlarm *AlarmManager::alarmAt(int index) const
{
    static UCAlarm alarm;
    Q_D(const AlarmManager);
    d->getAlarmAt(alarm, index);
    return &alarm;
}

UCAlarm *AlarmManager::findAlarm(const QVariant &cookie) const
{
    static UCAlarm alarm;
    Q_D(const AlarmManager);
    if (!d->findAlarm(alarm, cookie)) {
        return 0;
    };
    return &alarm;
}

bool AlarmManager::verifyChange(UCAlarm *alarm, Change change, const QVariant &newData)
{
    Q_D(AlarmManager);
    return d->verifyChange(alarm, change, newData);
}

bool AlarmManager::compareCookies(const QVariant &cookie1, const QVariant &cookie2)
{
    Q_D(AlarmManager);
    return d->compareCookies(cookie1, cookie2);
}

UCAlarmPrivate *AlarmManager::createAlarmData(UCAlarm *alarm)
{
    return AlarmManagerPrivate::get()->createAlarmData(alarm);
}

AlarmRequestPrivate *AlarmManager::createAlarmRequestData(AlarmRequest *request, bool autoDelete)
{
    return AlarmManagerPrivate::get()->createAlarmRequestData(request, autoDelete);
}

#include "moc_alarmmanager_p.cpp"
