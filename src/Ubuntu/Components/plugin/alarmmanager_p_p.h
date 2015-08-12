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

#ifndef ALARMMANAGER_P_H
#define ALARMMANAGER_P_H

#include "ucalarm.h"
#include "ucalarm_p.h"
#include "alarmmanager_p.h"
#include <QtCore/QUrl>

class AlarmManagerPrivate
{
    Q_DECLARE_PUBLIC(AlarmManager)
public:
    AlarmManagerPrivate(AlarmManager *qq);
    virtual ~AlarmManagerPrivate();

    static AlarmManagerPrivate *get(AlarmManager *instance = 0) {
        if (!instance) {
            return AlarmManager::instance().d_ptr.data();
        } else {
            return instance->d_ptr.data();
        }
    }

    AlarmManager *q_ptr;
    bool completed:1;

    virtual void init() = 0;
    virtual bool fetchAlarms() = 0;
    virtual int alarmCount() = 0;
    virtual UCAlarm *getAlarmAt(int index) const = 0;
    virtual bool findAlarm(const UCAlarm &alarm, const QVariant &cookie) const = 0;

    // function to verify whether the given alarm property has a given value set
    // used for testing purposes
    virtual bool verifyChange(UCAlarm *alarm, AlarmManager::Change change, const QVariant &value) = 0;

    // creates an alarm data adaptation object
    virtual UCAlarmPrivate *createAlarmData(UCAlarm *alarm) = 0;
};

AlarmManagerPrivate * createAlarmsAdapter(AlarmManager *alarms);

#endif // ALARMMANAGER_P_H
