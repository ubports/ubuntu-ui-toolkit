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

#ifndef UCALARM_P_H
#define UCALARM_P_H

#include "ucalarm.h"
#include "alarmmanager_p.h"
#include <QtCore/QDateTime>
#include <QtQml/QQmlListProperty>

class UCAlarmPrivate
{
    Q_DECLARE_PUBLIC(UCAlarm)
public:
    UCAlarmPrivate(UCAlarm *qq);
    ~UCAlarmPrivate();

    static UCAlarmPrivate *get(UCAlarm *alarm) {
        return alarm->d_func();
    }

    void setDefaults();

    UCAlarm* q_ptr;
    AlarmRequest *request;
    AlarmData rawData;
    int error;
    UCAlarm::Status status;

    // utility functions
    static UCAlarm::DayOfWeek dayOfWeek(const QDateTime &dt);
    static int firstDayOfWeek(UCAlarm::DaysOfWeek days);
    static int nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay);
    static bool multipleDaysSet(UCAlarm::DaysOfWeek days);
    UCAlarm::Error checkAlarm(int &changes);
    UCAlarm::Error checkDow(int &changes);
    UCAlarm::Error checkOneTime(int &changes);
    UCAlarm::Error checkRepeatingWeekly(int &changes);

    bool createRequest();

    // private slots
    void _q_syncStatus(int status, int error);
};

#endif // UUALARM_P_H
