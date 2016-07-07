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
#include <QtCore/QPointer>
#include <QtQml/QQmlListProperty>

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT UCAlarmPrivate
{
    Q_DECLARE_PUBLIC(UCAlarm)
public:
    UCAlarmPrivate(UCAlarm *qq);
    virtual ~UCAlarmPrivate();

    static UCAlarmPrivate *get(const UCAlarm *alarm) {
        return const_cast<UCAlarm*>(alarm)->d_ptr.data();
    }

    void setDefaults();

    // adaptation API
    virtual bool enabled() const = 0;
    virtual bool setEnabled(bool enabled) = 0;
    virtual QDateTime date() const = 0;
    virtual bool setDate(const QDateTime &date) = 0;
    virtual QString message() const = 0;
    virtual bool setMessage(const QString &message) = 0;
    virtual UCAlarm::AlarmType type() const = 0;
    virtual bool setType(UCAlarm::AlarmType type) = 0;
    virtual UCAlarm::DaysOfWeek daysOfWeek() const = 0;
    virtual bool setDaysOfWeek(UCAlarm::DaysOfWeek days) = 0;
    virtual QUrl sound() const = 0;
    virtual bool setSound(const QUrl &sound) = 0;
    virtual QVariant cookie() const = 0;
    virtual UCAlarm::Error checkAlarm() = 0;

    virtual void save() = 0;
    virtual void cancel() = 0;
    virtual void reset() = 0;
    virtual void completeSave() = 0;
    virtual void completeCancel() = 0;
    virtual void copyAlarmData(const UCAlarm &other) = 0;

    // common privates
    UCAlarm *q_ptr;
    unsigned int changes;
    int error;
    UCAlarm::Status status;

    // utility functions
    static UCAlarm::DayOfWeek dayOfWeek(const QDateTime &dt);
    static inline bool isDaySet(int dayNumber, UCAlarm::DaysOfWeek days);
    static int firstDayOfWeek(UCAlarm::DaysOfWeek days);
    static int nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay);
    static bool multipleDaysSet(UCAlarm::DaysOfWeek days);
    UCAlarm::Error adjustDow();
    UCAlarm::Error checkOneTime();
    UCAlarm::Error checkRepeatingWeekly();

    // private slots
    void _q_syncStatus(int operation, int status, int error);
};

inline bool UCAlarmPrivate::isDaySet(int dayNumber, UCAlarm::DaysOfWeek days)
{
    return (days & (1 << (dayNumber - 1))) == (1 << (dayNumber - 1));
}

UT_NAMESPACE_END

#endif // UUALARM_P_H
