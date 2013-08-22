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
#include "i18n.h"

#define IS_DAY_SET(bit, num)    (((num) & (1 << (bit - 1))) == (1 << (bit - 1)))

AlarmManagerPrivate::AlarmManagerPrivate(AlarmManager *qq)
    : QObject(qq)
    , completed(false)
    , q_ptr(qq)
{
}

AlarmManagerPrivate::~AlarmManagerPrivate()
{
}

UCAlarm::DayOfWeek AlarmManagerPrivate::dayOfWeek(const QDateTime &dt)
{
    return (UCAlarm::DayOfWeek)(1 << (dt.date().dayOfWeek() - 1));
}

int AlarmManagerPrivate::firstDayOfWeek(UCAlarm::DaysOfWeek days)
{
    for (int d = Qt::Monday; d <= Qt::Sunday; d++) {
        if ((1 << (d - 1)) & days) {
            return d;
        }
    }
    return 0;
}

int AlarmManagerPrivate::nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay)
{
    if (fromDay <= 0) {
        fromDay = Qt::Monday;
    }
    for (int d = fromDay; d <= Qt::Sunday; d++) {
        if ((1 << (d - 1)) & days) {
            return d;
        }
    }
    return 0;
}

// checks whether the given num has more than one bit set
bool AlarmManagerPrivate::multipleDaysSet(UCAlarm::DaysOfWeek days)
{
    unsigned bits;
    int num = static_cast<int>(days);
    for (bits = 0; num; bits++) {
        num &= num - 1; // clears the least significant bit
    }
    return (bits > 1);
}

void AlarmManagerPrivate::setRequestStatus(AlarmRequest *request, AlarmRequest::Status status, int error)
{
    request->setStatus(status, error);
}


UCAlarm::Error AlarmManagerPrivate::checkAlarm(AlarmData &alarm, int &changes)
{
    if (alarm.message.isEmpty()) {
        alarm.message = UbuntuI18n::instance().tr("Alarm");
        changes |= AlarmData::Message;
    }

    if (!alarm.date.isValid()) {
        return UCAlarm::InvalidDate;
    }

    // check type first as it may alter start day
    if (alarm.type == UCAlarm::OneTime) {
       return checkOneTime(alarm, changes);
    } else if (alarm.type == UCAlarm::Repeating) {
        return checkRepeatingWeekly(alarm, changes);
    }

    return UCAlarm::NoError;
}

UCAlarm::Error AlarmManagerPrivate::checkDow(AlarmData &alarm, int &changes)
{
    if (!alarm.days) {
        return UCAlarm::NoDaysOfWeek;
    } else if (alarm.days == UCAlarm::AutoDetect) {
        alarm.days = dayOfWeek(alarm.date);
        changes += AlarmData::Days;
    } else if (alarm.days != UCAlarm::Daily) {
        int alarmDay = firstDayOfWeek(alarm.days);
        int dayOfWeek = alarm.date.date().dayOfWeek();
        if (alarmDay < dayOfWeek) {
            alarm.date = alarm.date.addDays(7 - dayOfWeek + alarmDay);
        } else if (alarmDay > dayOfWeek) {
            alarm.date = alarm.date.addDays(alarmDay - dayOfWeek);
        }
        changes = AlarmData::Date;
    }
    return UCAlarm::NoError;
}

UCAlarm::Error AlarmManagerPrivate::checkOneTime(AlarmData &alarm, int &changes)
{
    // check days, days can be set for only one day in this case
    if (multipleDaysSet(alarm.days)) {
        return UCAlarm::OneTimeOnMoreDays;
    }

    // adjust start date and/or dayOfWeek according to their values
    UCAlarm::Error result = checkDow(alarm, changes);
    if (result != UCAlarm::NoError) {
        return result;
    }

    // start date should be later then the current date/time
    if (alarm.date <= QDateTime::currentDateTime()) {
        return UCAlarm::EarlyDate;
    }
    return UCAlarm::NoError;
}

UCAlarm::Error AlarmManagerPrivate::checkRepeatingWeekly(AlarmData &alarm, int &changes)
{
    // start date is adjusted depending on the days value;
    // start date can be set to be the current time, as scheduling will move
    // it to the first occurence.
    UCAlarm::Error result = checkDow(alarm, changes);
    if (result != UCAlarm::NoError) {
        return result;
    }

    // move start time to the first occurence if needed
    int dayOfWeek = alarm.date.date().dayOfWeek();
    if (!IS_DAY_SET(dayOfWeek, alarm.days) || (alarm.date <= QDateTime::currentDateTime())) {
        // check the next occurence of the alarm
        int nextOccurence = nextDayOfWeek(alarm.days, dayOfWeek);
        if (nextOccurence <= 0) {
             // the starting date should be moved to the next week
            nextOccurence = firstDayOfWeek(alarm.days);
            alarm.date.addDays(7 - dayOfWeek + nextOccurence);
        } else {
            // the starting date is still this week
            alarm.date.addDays(nextOccurence - dayOfWeek);
        }
        changes += AlarmData::Date;
    }

    return UCAlarm::NoError;
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

AlarmRequest *AlarmManager::createRequest(QObject *owner, bool autoDelete)
{
    return createAlarmRequest(owner, autoDelete);
}


/*
 * Updates or adds an alarm to the alarm collection. The owner must have
 * operationResult(status, errorCode) slot,
 * where the actual status of the operation is reported.
 *
 * Returns the result code of the operation.
 */
void AlarmManager::set(AlarmRequest *request, AlarmData &alarm, int &changes)
{
    changes = 0;
    if (!request) {
        return;
    }

    Q_D(AlarmManager);

    UCAlarm::Error result = d->checkAlarm(alarm, changes);
    if (result != UCAlarm::NoError) {
        d->setRequestStatus(request, AlarmRequest::Fail, result);
    } else {
        if (!alarm.cookie.isValid()) {
            d->addAlarm(request, alarm);
        } else {
            d->updateAlarm(request, alarm);
        }
    }
}

/*
 * Cancels an alarm. Returns the result of the operation;
 */
void AlarmManager::cancel(AlarmRequest *request, AlarmData &alarm)
{
    if (!request) {
        return;
    }

    Q_D(AlarmManager);

    if (!alarm.cookie.isValid()) {
        d->setRequestStatus(request, AlarmRequest::Fail, UCAlarm::InvalidEvent);
    } else {
        d->removeAlarm(request, alarm);
    }
}

AlarmRequest::AlarmRequest(bool autoDelete, QObject *parent)
    : QObject(parent)
    , m_status(Ready)
    , m_error(UCAlarm::NoError)
    , m_completed(true)
    , m_autoDelete(autoDelete)
{
}
AlarmRequest::~AlarmRequest()
{
}
