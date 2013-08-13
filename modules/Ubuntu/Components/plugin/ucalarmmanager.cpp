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

#include "ucalarmmanager.h"
#include "ucalarmmanager_p.h"
#include "ucalarm.h"
#include "ucalarm_p.h"
#include "i18n.h"

#include <QDebug>

#define IS_DAY_SET(bit, num)    (((num) & (1 << (bit - 1))) == (1 << (bit - 1)))

UCAlarmManagerPrivate::UCAlarmManagerPrivate(UCAlarmManager *qq)
    : QObject(qq)
    , q_ptr(qq)
    , completed(false)
{
}

UCAlarmManagerPrivate::~UCAlarmManagerPrivate()
{
    clear();
}

void UCAlarmManagerPrivate::clear()
{
    if (alarmList.count()) {
        Q_FOREACH(UCAlarm *alarm, alarmList) {
            delete alarm;
        }
        alarmList.clear();
    }

}

void UCAlarmManagerPrivate::error(UCAlarmManager::Error code, const QString &msg)
{
    errorMessage = msg;
    errorCode = code;
    qWarning() << msg;
}

void UCAlarmManagerPrivate::resetError()
{
    errorCode = UCAlarmManager::NoError;
    errorMessage.clear();
}

UCAlarm::DayOfWeek UCAlarmManagerPrivate::dayOfWeek(const QDateTime &dt)
{
    return (UCAlarm::DayOfWeek)(1 << (dt.date().dayOfWeek() - 1));
}

int UCAlarmManagerPrivate::firstDayOfWeek(UCAlarm::DaysOfWeek days)
{
    for (int d = Qt::Monday; d <= Qt::Sunday; d++) {
        if ((1 << (d - 1)) & days) {
            return d;
        }
    }
    return 0;
}

int UCAlarmManagerPrivate::nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay)
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
bool UCAlarmManagerPrivate::multipleDaysSet(UCAlarm::DaysOfWeek days)
{
    unsigned bits;
    int num = static_cast<int>(days);
    for (bits = 0; num; bits++) {
        num &= num - 1; // clears the least significant bit
    }
    return (bits > 1);
}



bool UCAlarmManagerPrivate::checkAlarm(RawAlarm &alarm)
{
    if (!alarm.date.isValid()) {
        error(UCAlarmManager::InvalidDate, "Invalid start date given");
        return false;
    }

    // check type first as it may alter start day
    if (((alarm.type == UCAlarm::OneTime) && !checkOneTime(alarm)) ||
        ((alarm.type == UCAlarm::Repeating) && !checkRepeatingWeekly(alarm))) {
        return false;
    }

    return true;
}

bool UCAlarmManagerPrivate::checkDow(RawAlarm &alarm)
{
    if (!alarm.days) {
        error(UCAlarmManager::NoDaysOfWeek, "Days of week wasn't specified for the recurring alarm.");
        return false;
    } else if (alarm.days == UCAlarm::AutoDetect) {
        alarm.days = dayOfWeek(alarm.date);
    } else if (alarm.days != UCAlarm::Daily) {
        int alarmDay = firstDayOfWeek(alarm.days);
        int dayOfWeek = alarm.date.date().dayOfWeek();
        if (alarmDay < dayOfWeek) {
            alarm.date = alarm.date.addDays(7 - dayOfWeek + alarmDay);
        } else if (alarmDay > dayOfWeek) {
            alarm.date = alarm.date.addDays(alarmDay - dayOfWeek);
        }
    }
    return true;
}

bool UCAlarmManagerPrivate::checkOneTime(RawAlarm &alarm)
{
    // check days, days can be set for only one day in this case
    if (multipleDaysSet(alarm.days)) {
        error(UCAlarmManager::OneTimeOnMoreDays, "Multiple alarm days given for a one time alarm");
        return false;
    }

    // adjust start date and/or dayOfWeek according to their values
    if (!checkDow(alarm)) {
        return false;
    }

    // start date should be later then the current date/time
    if (alarm.date <= QDateTime::currentDateTime()) {
        error(UCAlarmManager::EarlyDate, "Start date is earlier than the current time.");
        return false;
    }
    return true;
}

bool UCAlarmManagerPrivate::checkRepeatingWeekly(RawAlarm &alarm)
{
    // start date is adjusted depending on the days value;
    // start date can be set to be the current time, as scheduling will move
    // it to the first occurence.
    if (!checkDow(alarm)) {
        return false;
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
    }

    return true;
}


/*
 * Slot refreshing the clock alarms.
 */
void UCAlarmManagerPrivate::refreshAlarms()
{
    Q_Q(UCAlarmManager);

    // clean previous alarms
    clear();
    // create new list
    QList<RawAlarm> rawList = getAlarms();
    Q_FOREACH(RawAlarm rawData, rawList) {
        UCAlarm *alarm = new UCAlarm;
        UCAlarmPrivate *palarm = UCAlarmPrivate::get(alarm);
        palarm->rawData = rawData;
        alarmList << alarm;
    }

    if (completed) {
        Q_EMIT q->alarmsChanged();
    }
}

QQmlListProperty<UCAlarm> UCAlarmManagerPrivate::alarms()
{
    return QQmlListProperty<UCAlarm>(q_func(), alarmList);
}

/*!
 * \qmltype Alarms
 * \instantiates UCAlarmManager
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu-services
 * \brief Alarms provides alarm scheduling functions.
 */

UCAlarmManager::UCAlarmManager(QObject *parent)
    : QObject(parent)
    , d_ptr(createAlarmsAdapter(this))
{
    d_ptr->refreshAlarms();
    d_ptr->completed = true;
}

UCAlarmManager::~UCAlarmManager()
{
}

QList<UCAlarm*> UCAlarmManager::alarms()
{
    Q_D(UCAlarmManager);
    return d->alarmList;
}

UCAlarmManager::Error UCAlarmManager::error() const
{
    Q_D(const UCAlarmManager);
    return d->errorCode;
}
QString UCAlarmManager::errorMessage() const
{
    Q_D(const UCAlarmManager);
    return d->errorMessage;
}


/*!
 * \qmlmethod bool Alarms::set(Alarm alarm)
 * Updates or adds an alarm to the alarm collection.
 */
bool UCAlarmManager::set(UCAlarm *alarm)
{
    if (!alarm) {
        return false;
    }

    Q_D(UCAlarmManager);
    d->resetError();
    UCAlarmPrivate *palarm = UCAlarmPrivate::get(alarm);

    if (!d->checkAlarm(palarm->rawData)) {
        return false;
    }

    if (!palarm->rawData.cookie.isValid()) {
        return d->addAlarm(palarm->rawData);
    } else {
        return d->updateAlarm(palarm->rawData);
    }
}

/*!
 * \qmlmethod bool Alarms::setOneTime(Date date, string message)
 * The function creates a one time alarm with the given start \a date and \a message.
 * The alarm created will have the default tone. Returns true on success, and on error
 * the error code will be reported in \l error and \l errorMessage properties.
 */
bool UCAlarmManager::setOneTime(const QDateTime &date, const QString &message)
{
    RawAlarm alarm;
    alarm.date = date;
    alarm.message = message;
    if (alarm.message.isEmpty()) {
        alarm.message = UbuntuI18n::instance().tr("Alarm");
    }

    Q_D(UCAlarmManager);
    d->resetError();
    if (!d->checkAlarm(alarm)) {
        return false;
    }
    return d->addAlarm(alarm);
}
/*!
 * \qmlmethod bool Alarms::setRepeating(Date date, DaysOfWeek days, string message)
 * The function creates a recurring alarm to be kicked on the given \a date, with the given
 * \a type and on the specified \a daysOfWeek.
 */
bool UCAlarmManager::setRepeating(const QDateTime &date, int days, const QString &message)
{
    Q_D(UCAlarmManager);
    d->resetError();

    RawAlarm alarm;
    alarm.date = date;
    alarm.message = message;
    alarm.type = UCAlarm::Repeating;
    alarm.days = static_cast<UCAlarm::DaysOfWeek>(days);

    if (!d->checkAlarm(alarm)) {
        return false;
    }
    return d->addAlarm(alarm);
}

/*!
 * \qmlmethod bool Alarms::cancel(Alarm alarm)
 */
bool UCAlarmManager::cancel(UCAlarm *alarm)
{
    if (!alarm) {
        return false;
    }
    Q_D(UCAlarmManager);
    d->resetError();

    UCAlarmPrivate *palarm = UCAlarmPrivate::get(alarm);

    if (!palarm->rawData.cookie.isValid()) {
        d->error(UCAlarmManager::InvalidEvent, "The alarm event is not registered.");
        return false;
    }
    return d->removeAlarm(palarm->rawData);
}

#include "moc_ucalarmmanager.cpp"
