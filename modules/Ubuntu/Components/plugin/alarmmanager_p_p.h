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

#include "alarmmanager_p.h"
#include <QtCore/QUrl>

class AlarmManagerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AlarmManager)
public:
    AlarmManagerPrivate(AlarmManager *qq);
    virtual ~AlarmManagerPrivate();

    QList<AlarmData> alarmList;
    bool completed:1;

    // utility functions
    static UCAlarm::DayOfWeek dayOfWeek(const QDateTime &dt);
    static int firstDayOfWeek(UCAlarm::DaysOfWeek days);
    static int nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay);
    static bool multipleDaysSet(UCAlarm::DaysOfWeek days);

    // adaptation methods
    virtual int addAlarm(AlarmData &alarm) = 0;
    virtual int updateAlarm(AlarmData &alarm) = 0;
    virtual int removeAlarm(AlarmData &alarm) = 0;
protected Q_SLOTS:
    virtual void fetchAlarms() = 0;

protected:
    AlarmManager *q_ptr;

private:
    UCAlarm::Error checkAlarm(AlarmData &alarm, int &changes);
    UCAlarm::Error checkDow(AlarmData &alarm, int &changes);
    UCAlarm::Error checkOneTime(AlarmData &alarm, int &changes);
    UCAlarm::Error checkRepeatingWeekly(AlarmData &alarm, int &changes);
};

AlarmManagerPrivate * createAlarmsAdapter(AlarmManager *alarms);

#endif // ALARMMANAGER_P_H
