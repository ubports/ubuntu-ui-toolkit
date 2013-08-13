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
 * \qmltype AlarmManager
 * \instantiates UCAlarmManager
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu-services
 * \brief AlarmManager provides alarm management functions.
 *
 * AlarmManager is a singleton component which provides functions to list, create,
 * update and cancel alarms. Alarm objects are listed in \l alarms property, where
 * each alarm object embeds an Alarm element.
 *
 * Alarms can be created using the \l set function, where the alarm data is presented
 * in an Alarm element, or by using one of the helper functions - \l setOneTime or
 * \l setRepeating. An alarm event can be cancelled using \l cancel function.
 *
 * The following code snippet illustrates listing of alarm events.
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 * import Ubuntu.Components.ListItems 0.1 as ListItem
 * ListView {
 *     model: AlarmManager.alarms
 *     delegate: ListItem.Subtitled {
 *         text: modelData.message
 *         subText: Qt.formatDateTime(modelData.date);
 *     }
 * }
 * \endqml
 *
 * Errors occured during each operation are reported in \l error property, and in a
 * clear readable text format in \l errorMessage properties.
 *
 * \table
 *   \header
 *      \li Error code
 *      \li Value
 *      \li Description
 *   \row
 *      \li NoError
 *      \li 0
 *      \li Successful operation completion.
 *   \row
 *      \li InvalidDate
 *      \li -1
 *      \li The date specified for the alarm was invalid.
 *   \row
 *      \li EarlyDate
 *      \li -2
 *      \li The date specified for the alarm is an earlier date than the current one.
 *   \row
 *      \li NoDaysOfWeek
 *      \li -3
 *      \li The daysOfWeek parameter of the alarm was not specified.
 *   \row
 *      \li OneTimeOnMoreDays
 *      \li -4
 *      \li The one-time alarm was set to be kicked in several days.
 *   \row
 *      \li InvalidEvent
 *      \li -5
 *      \li The alarm event is invalid.
 *   \row
 *      \li AdaptationError
 *      \li -100
 *      \li The error occurred in alarm adaptation layer.
 * \endtable
 *
 */

/*!
 * \qmlproperty list<Alarm> AlarmManager::alarms
 * The property contains the alarm events registered. The property is updated
 * whenever a new alarm is added, updated or removed (cancelled). This property
 * can be bound to list models in QML.
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

/*!
 * \qmlproperty Error AlarmManager::error
 * The property holds the error code occurred during the last performed operation.
 */
UCAlarmManager::Error UCAlarmManager::error() const
{
    Q_D(const UCAlarmManager);
    return d->errorCode;
}
/*!
 * \qmlproperty string AlarmManager::errorMessage
 * The property holds the error occurred iduring the last operation in human readable
 * form. On no error the property holds an empty string.
 */
QString UCAlarmManager::errorMessage() const
{
    Q_D(const UCAlarmManager);
    return d->errorMessage;
}


/*!
 * \qmlmethod bool AlarmManager::set(Alarm alarm)
 * Updates or adds an alarm to the alarm collection. The alarm event can be one
 * time or repeating type, and is checked agains consistency (alarm type, date
 * and occurrence days) before it is added or updated to the alarm collection.
 * For instance a one time alarm (Alarm.type = Alarm.OneTime) alarm cannot have daysOfWeek
 * set for other value than Alarm.AutoDetect) and cannot have its date set to
 * an earlier time than the current one.
 *
 * The function also aligns alarm fields to match the first occurence. In this
 * way the \a alarm fields will be altered as follows:
 * \list
 *  \li - the alarm daysOfWeek will be set to the alarm day if the daysOfWeek is set
 *      to Alarm.AutoDetect.
 *  \li - if the daysOfWeek is set to a day other than the one specified in the date
 *      field, the date will be moved ahead to match the day from the daysOfWeek.
 * \endlist
 *
 *
 * The function will fail if
 * \list
 *  \li - the date field is invalid
 *  \li - for one time alarm, the date field is earlier than the current time
 *  \li - the daysOfWeek is set to multiple days for one time alarm
 * \endlist
 *
 * Returns true if the alarm was successfully saved, and on failure the \a error
 * property holds the failure code.
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
 * \qmlmethod bool AlarmManager::setOneTime(Date date, string message)
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
 * \qmlmethod bool AlarmManager::setRepeating(Date date, DaysOfWeek days, string message)
 * The function creates a repeating alarm to be kicked on the given \a date on the
 * specified \a days. Returns true on success, false on error.
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
 * \qmlmethod bool AlarmManager::cancel(Alarm alarm)
 * The function cancels (removes) an alarm event from the alarms register. Returns
 * true on successful operation, false on error. The \l error presents the code of
 * the failure.
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
