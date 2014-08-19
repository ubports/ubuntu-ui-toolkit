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

    static UCAlarmPrivate *get(const UCAlarm *alarm) {
        return const_cast<UCAlarm*>(alarm)->d_func();
    }

    void setDefaults();

    UCAlarm* q_ptr;
    AlarmRequest *request;
    AlarmData rawData;
    int error;
    UCAlarm::Status status;

    // utility functions
    static UCAlarm::DayOfWeek dayOfWeek(const QDateTime &dt);
    static inline bool isDaySet(int dayNumber, UCAlarm::DaysOfWeek days);
    static int firstDayOfWeek(UCAlarm::DaysOfWeek days);
    static int nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay);
    static bool multipleDaysSet(UCAlarm::DaysOfWeek days);
    UCAlarm::Error checkAlarm();
    UCAlarm::Error adjustDow();
    UCAlarm::Error checkOneTime();
    UCAlarm::Error checkRepeatingWeekly();

    bool createRequest();

    // private slots
    void _q_syncStatus(int operation, int status, int error);
};

inline bool UCAlarmPrivate::isDaySet(int dayNumber, UCAlarm::DaysOfWeek days)
{
    return (days & (1 << (dayNumber - 1))) == (1 << (dayNumber - 1));
}

#endif // UUALARM_P_H
