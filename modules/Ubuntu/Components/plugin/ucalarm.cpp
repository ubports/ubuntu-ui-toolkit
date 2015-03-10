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

#include "ucalarm.h"
#include "ucalarm_p.h"
#include "alarmmanager_p.h"
#include "i18n.h"
#include <QtQml/QQmlInfo>

UCAlarmPrivate::UCAlarmPrivate(UCAlarm *qq)
    : q_ptr(qq)
    , changes(0)
    , error(UCAlarm::NoError)
    , status(UCAlarm::Ready)
{
}

UCAlarmPrivate::~UCAlarmPrivate()
{
}

void UCAlarmPrivate::setDefaults()
{
    QDateTime date = AlarmUtils::normalizeDate(QDateTime::currentDateTime());
    setDate(date);
    setMessage(UbuntuI18n::instance().tr("Alarm"));
    setType(UCAlarm::OneTime);
    setDaysOfWeek(UCAlarm::AutoDetect);
}

void UCAlarmPrivate::_q_syncStatus(int operation, int status, int error) {
    UCAlarm::Status alarmStatus = static_cast<UCAlarm::Status>(status);
    if (this->status != alarmStatus || this->error != error) {
        this->status = alarmStatus;
        this->error = error;

        Q_Q(UCAlarm);
        if (this->status == UCAlarm::Ready) {
            // sync field changes occured during operation
            if (changes & AlarmManager::Enabled)
                Q_EMIT q->enabledChanged();
            if (changes & AlarmManager::Date)
                Q_EMIT q->dateChanged();
            if (changes & AlarmManager::Message)
                Q_EMIT q->messageChanged();
            if (changes & AlarmManager::Sound)
                Q_EMIT q->soundChanged();
            if (changes & AlarmManager::Type)
                Q_EMIT q->typeChanged();
            if (changes & AlarmManager::Days)
                Q_EMIT q->daysOfWeekChanged();
            changes = 0;
        }

        Q_EMIT q->statusChanged(static_cast<UCAlarm::Operation>(operation));
        Q_EMIT q->errorChanged();
    }
}

UCAlarm::DayOfWeek UCAlarmPrivate::dayOfWeek(const QDateTime &dt)
{
    return (UCAlarm::DayOfWeek)(1 << (dt.date().dayOfWeek() - 1));
}

int UCAlarmPrivate::firstDayOfWeek(UCAlarm::DaysOfWeek days)
{
    for (int d = Qt::Monday; d <= Qt::Sunday; d++) {
        if ((1 << (d - 1)) & days) {
            return d;
        }
    }
    return 0;
}

int UCAlarmPrivate::nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay)
{
    if (fromDay <= 0 || fromDay >= Qt::Sunday) {
        // start from the beginning of the week
        fromDay = Qt::Monday;
    } else {
        // start checking from the next day onwards
        fromDay++;
    }
    for (int d = fromDay; d <= Qt::Sunday; d++) {
        if ((1 << (d - 1)) & days) {
            return d;
        }
    }

    // none found for the rest of the week, return the fist day set
    return firstDayOfWeek(days);
}

// checks whether the given num has more than one bit set
bool UCAlarmPrivate::multipleDaysSet(UCAlarm::DaysOfWeek days)
{
    unsigned bits;
    int num = static_cast<int>(days);
    for (bits = 0; num; bits++) {
        num &= num - 1; // clears the least significant bit
    }
    return (bits > 1);
}

// adjust dayOfWeek
UCAlarm::Error UCAlarmPrivate::adjustDow()
{
    UCAlarm::DaysOfWeek dow = daysOfWeek();
    if (!dow) {
        return UCAlarm::NoDaysOfWeek;
    } else if (dow == UCAlarm::AutoDetect) {
        setDaysOfWeek(dayOfWeek(date()));
        changes |= AlarmManager::Days;
    }
    return UCAlarm::NoError;
}

UCAlarm::Error UCAlarmPrivate::checkOneTime()
{
    // check days, days can be set for only one day in this case
    UCAlarm::DaysOfWeek dow = daysOfWeek();
    if (multipleDaysSet(dow)) {
        return UCAlarm::OneTimeOnMoreDays;
    }

    UCAlarm::Error result = adjustDow();
    if (result != UCAlarm::NoError) {
        return result;
    }
    // fetch the dow again as adjustment may alter it!
    dow = daysOfWeek();

    // fix lp:1319401 - dayOfWeek omitted if set to other than AutoDetect
    QDateTime alarmDate = date();
    int dayOfWeek = alarmDate.date().dayOfWeek();
    if (!isDaySet(dayOfWeek, dow)) {
        // dayOfWeek has been set by the user, adjust the date to it
        int nextOccurrence = nextDayOfWeek(dow, dayOfWeek);
        if (nextOccurrence < dayOfWeek) {
             // the starting date should be moved to the next week's occurrence
            alarmDate = alarmDate.addDays(7 - dayOfWeek + nextOccurrence);
        } else {
            // the starting date is still this week
            alarmDate = alarmDate.addDays(nextOccurrence - dayOfWeek);
        }
        setDate(alarmDate);
        changes |= AlarmManager::Date;
    }

    // start date should be later then the current date/time
    if (alarmDate <= AlarmUtils::normalizeDate(QDateTime::currentDateTime())) {
        return UCAlarm::EarlyDate;
    }
    return UCAlarm::NoError;
}

UCAlarm::Error UCAlarmPrivate::checkRepeatingWeekly()
{
    // start date is adjusted depending on the days value;
    // start date can be set to be the current time, as scheduling will move
    // it to the first occurence.
    UCAlarm::Error result = adjustDow();
    if (result != UCAlarm::NoError) {
        return result;
    }

    // move start time of the first occurence if needed
    QDateTime alarmDate = date();
    UCAlarm::DaysOfWeek dow = daysOfWeek();
    int dayOfWeek = alarmDate.date().dayOfWeek();
    if (!isDaySet(dayOfWeek, dow) || (alarmDate <= QDateTime::currentDateTime())) {
        // check the next occurence of the alarm
        int nextOccurrence = nextDayOfWeek(dow, dayOfWeek);
        if (nextOccurrence == dayOfWeek) {
            // move the date to the same day next week
            alarmDate = alarmDate.addDays(7);
        } else if (nextOccurrence < dayOfWeek) {
             // the starting date should be moved to the next week's occurrence
            alarmDate = alarmDate.addDays(7 - dayOfWeek + nextOccurrence);
        } else {
            // the starting date is still this week
            alarmDate = alarmDate.addDays(nextOccurrence - dayOfWeek);
        }
        setDate(alarmDate);
        changes |= AlarmManager::Date;
    }

    return UCAlarm::NoError;
}


/*!
 * \qmltype Alarm
 * \instantiates UCAlarm
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu-services
 * \brief Alarm component is a representation of an alarm event.
 *
 * The Alarm element encapsulates alarm event data. Supports one time and repeating
 * alarms, where repeating can be either daily or weekly on one or several selected
 * days.
 *
 * The alarm data is validated upon \l save operation, which can be used to save
 * a new alarm or update existing alarm's properties.
 *
 * During data validation the alarm properties may suffer changes. These changes
 * will be reported back to each changed property. See what changes can occurr at
 * \l save function documentation.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * Rectangle {
 *     width: units.gu(40)
 *     height: units.gu(20)
 *
 *     Alarm{
 *         id: alarm
 *     }
 *     Column {
 *         spacing: units.gu(1)
 *         Row {
 *             spacing: units.gu(1)
 *             Label {
 *                 id: date
 *                 text: "Date:"
 *                 anchors.verticalCenter: parent.verticalCenter
 *             }
 *             TextField {
 *                 text: alarm.date.toString()
 *                 onAccepted: alarm.date = new Date(text)
 *             }
 *         }
 *         Row {
 *             spacing: units.gu(1)
 *             Label {
 *                 id: msg
 *                 text: "Message:"
 *                 anchors.verticalCenter: parent.verticalCenter
 *             }
 *             TextField {
 *                 text: alarm.message
 *                 onAccepted: alarm.message = text
 *             }
 *         }
 *         Button {
 *             text: "Save"
 *             onClicked: {
 *                 alarm.save();
 *                 if (alarm.error != Alarm.NoError)
 *                     print("Error saving alarm, code: " + alarm.error);
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * An alarm can be cancelled using \l cancel function but only if the event has
 * previously been stored in the alarm collection.
 *
 * The \l reset function clears the properties of the alarm bringing them to the
 * default values. In this way the same alarm component can be used to save several
 * alarms at the same time.
 * \note Do not call reset function on an alarm event object when that was returned
 * by the \l AlarmModel::get function, as that will reset the alarm cache data!
 */

UCAlarm::UCAlarm(QObject *parent)
    : QObject(parent)
    , d_ptr(AlarmManager::createAlarmData(this))
{
    d_ptr->setDefaults();
}

UCAlarm::UCAlarm(const QDateTime &dt, const QString &message, QObject *parent)
    : QObject(parent)
    , d_ptr(AlarmManager::createAlarmData(this))
{
    d_ptr->setDate(AlarmUtils::normalizeDate(dt));
    if (!message.isEmpty()) {
        d_ptr->setMessage(message);
    }
    d_ptr->setDaysOfWeek(UCAlarmPrivate::dayOfWeek(AlarmUtils::normalizeDate(dt)));
}

UCAlarm::UCAlarm(const QDateTime &dt, DaysOfWeek days, const QString &message, QObject *parent)
    : QObject(parent)
    , d_ptr(AlarmManager::createAlarmData(this))
{
    d_ptr->setDate(AlarmUtils::normalizeDate(dt));
    d_ptr->setType(Repeating);
    if (days == AutoDetect) {
        d_ptr->setDaysOfWeek(UCAlarmPrivate::dayOfWeek(dt));
    } else {
        d_ptr->setDaysOfWeek(days);
    }
    if (!message.isEmpty()) {
        d_ptr->setMessage(message);
    }
}

UCAlarm::~UCAlarm()
{
}

bool UCAlarm::operator==(const UCAlarm &that) const
{
    return date() == that.date() &&
            message() == that.message() &&
            type() == that.type() &&
            daysOfWeek() == that.daysOfWeek();
}

/*!
 * \qmlproperty bool Alarm::enabled
 * The property specifies whether the alarm is enabled or not. Disable dalarms
 * are not scheduled.
 * The default value is true;
 */
bool UCAlarm::enabled() const
{
    return d_ptr->enabled();
}
void UCAlarm::setEnabled(bool enabled)
{
    if (d_ptr->setEnabled(enabled)) {
        d_ptr->changes |= AlarmManager::Enabled;
        Q_EMIT enabledChanged();
    }
}

/*!
 * \qmlproperty Date Alarm::date
 * The property holds the date the alarm will be triggered. The default
 * value is the current date and time the alarm object was created. Further
 * \l reset calls will bring the value back to the time the \l reset was called.
 */
QDateTime UCAlarm::date() const
{
    return d_ptr->date();
}
void UCAlarm::setDate(const QDateTime &date)
{
    if (d_ptr->setDate(AlarmUtils::normalizeDate(date))) {
        d_ptr->changes |= AlarmManager::Date;
        Q_EMIT dateChanged();
        return;
    }
}

/*!
 * \qmlproperty string Alarm::message
 * The property holds the message string which will be displayed when the alarm
 * is triggered. The default value is the localized "Alarm" text.
 */
QString UCAlarm::message() const
{
    return d_ptr->message();
}
void UCAlarm::setMessage(const QString &message)
{
    if (d_ptr->setMessage(message)) {
        d_ptr->changes |= AlarmManager::Message;
        Q_EMIT messageChanged();
    }
}

/*!
 * \qmlproperty AlarmType Alarm::type
 * The property holds the type of the alarm, which can have one of the
 * following values:
 * \table
 * \header
 *  \li {2, 1} Alarm.AlarmType enum:
 * \header
 *  \li Type
 *  \li Description
 * \row
 *  \li Alarm.OneTime
 *  \li The alarm occurs only once.
 * \row
 *  \li Alarm.Repeating
 *  \li The alarm is a repeating one, either daily, weekly on a given day or on
 *      selected days.
 * \endtable
 *
 *The default value is Alarm.OneTime.
 */
UCAlarm::AlarmType UCAlarm::type() const
{
    return d_ptr->type();
}
void UCAlarm::setType(UCAlarm::AlarmType type)
{
    if (d_ptr->setType(type)) {
        d_ptr->changes |= AlarmManager::Type;
        Q_EMIT typeChanged();
    }
}

/*!
 * \qmlproperty DaysOfWeek Alarm::daysOfWeek
 * The property holds the days of the week the alarm is scheduled. This property
 * can have only one day set for one time alarms and multiple days for repeating
 * alarms.
 * The following flags can be set:
 * \table
 * \header
 *  \li {3, 1} Alarm.DayOfWeek enum:
 * \header
 *  \li Type
 *  \li Value
 *  \li Description
 * \row
 *  \li Alarm.Monday
 *  \li 0x01
 *  \li The alarm will kick on Mondays.
 * \row
 *  \li Alarm.Tuesday
 *  \li 0x02
 *  \li The alarm will kick on Tuesdays.
 * \row
 *  \li Alarm.Wednesday
 *  \li 0x04
 *  \li The alarm will kick on Wednesdays.
 * \row
 *  \li Alarm.Thursday
 *  \li 0x08
 *  \li The alarm will kick on Thursdays.
 * \row
 *  \li Alarm.Friday
 *  \li 0x10
 *  \li The alarm will kick on Fridays.
 * \row
 *  \li Alarm.Saturday
 *  \li 0x20
 *  \li The alarm will kick on Saturdays.
 * \row
 *  \li Alarm.Sunday
 *  \li 0x40
 *  \li The alarm will kick on Sundays.
 * \row
 *  \li Alarm.AutoDetect
 *  \li 0x80
 *  \li The alarm day will be detected from the alarm date at the time the alarm
 *      is saved.
 * \endtable
 *
 * The default value is Alarm.AutoDetect.
 */
UCAlarm::DaysOfWeek UCAlarm::daysOfWeek() const
{
    return d_ptr->daysOfWeek();
}
void UCAlarm::setDaysOfWeek(UCAlarm::DaysOfWeek days)
{
    if (d_ptr->setDaysOfWeek(days)) {
        d_ptr->changes |= AlarmManager::Days;
        Q_EMIT daysOfWeekChanged();
    }
}

/*!
 * \qmlproperty url Alarm::sound
 * The property holds the alarm's sound to be played when the alarm is triggered.
 * An empty url will mean to play the default sound.
 *
 * The defaul tvalue is an empty url.
 */
QUrl UCAlarm::sound() const
{
    return d_ptr->sound();
}
void UCAlarm::setSound(const QUrl &sound)
{
    if (d_ptr->setSound(sound)) {
        d_ptr->changes |= AlarmManager::Sound;
        Q_EMIT soundChanged();
    }
}

/*!
 * \qmlproperty Error Alarm::error
 * The property holds the error code occurred during the last performed operation.
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
 *      \li 1
 *      \li The date specified for the alarm was invalid.
 *   \row
 *      \li EarlyDate
 *      \li 2
 *      \li The date specified for the alarm is an earlier date than the current one.
 *   \row
 *      \li NoDaysOfWeek
 *      \li 3
 *      \li The daysOfWeek parameter of the alarm was not specified.
 *   \row
 *      \li OneTimeOnMoreDays
 *      \li 4
 *      \li The one-time alarm was set to be kicked in several days.
 *   \row
 *      \li InvalidEvent
 *      \li 5
 *      \li The alarm event is invalid.
 *   \row
 *      \li AdaptationError
 *      \li 100
 *      \li The error occurred in alarm adaptation layer. Adaptations may define
 *          additional behind this value.
 * \endtable
 */
int UCAlarm::error() const
{
    return d_ptr->error;
}

/*!
 * \qmlproperty Status Alarm::status
 * The property holds the status of the last performed operation. It can take one
 * of the following values:
 * \table
 *   \header
 *      \li Status code
 *      \li Value
 *      \li Description
 *   \row
 *      \li Ready
 *      \li 1
 *      \li Specifies either that the Alarm object is ready to perform any operation
 *          or that the previous operation has been successfully completed.
 *   \row
 *      \li InProgress
 *      \li 2
 *      \li Specifies that there is an operation pending on Alarm object.
 *   \row
 *      \li Fail
 *      \li 3
 *      \li Specifies that the last alarm operation has failed. The failure code is
 *          set in \l error property.
 * \endtable
 *
 * The notification signal has a parameter specifying the \a operation the status
 * refers to. The operation can take the following values:
 * \table
 *   \header
 *      \li Operation code
 *      \li Description
 *   \row
 *      \li NoOperation
 *      \li There is no operation pending. This may be set when an error occured
 *          in the alarm adapters and the operation cannot be determined.
 *  \row
 *      \li Saving
 *      \li The status reported refers to an operation requested through save().
 *  \row
 *      \li Canceling
 *      \li The status reported refers to an operation requested through cancel().
 *  \row
 *      \li Reseting
 *      \li The status reported refers to an operation requested through reset().
 * \endtable
 *
 * For example an implementation which resets the alarm data whenever
 * the save or cancel operations succeed would look as follows:
 *
 * \qml
 * Alarm {
 *     onStatusChanged: {
 *         if (status !== Alarm.Ready)
 *             return;
 *         if ((operation > Alarm.NoOperation) && (operation < Alarm.Reseting)) {
 *             reset();
 *         }
 *     }
 * }
 * \endqml
 */
UCAlarm::Status UCAlarm::status() const
{
    return d_ptr->status;
}

/*!
 * \qmlmethod Alarm::save()
 * Updates or adds an alarm to the alarm collection. The operation aligns properties
 * according to the following rules:
 * \list
 *  \li - the \l daysOfWeek will be set to the alarm day if the daysOfWeek was set
 *      to Alarm.AutoDetect.
 *  \li - if the \l daysOfWeek is set to a day other than the one specified in the \l date
 *      field, the \l date will be moved ahead to match the day from the \l daysOfWeek.
 * \endlist
 *
 * The function will fail if
 * \list
 *  \li - the \l date property is invalid
 *  \li - for one time alarm, the \l date property falue is earlier than the current time
 *  \li - the \l daysOfWeek property is set to multiple days for one time alarm
 * \endlist
 *
 * The operation is asynchronous, and its status is reported through the \l status
 * property. Further operations should wait till the previous operation is completed.
 * The operation result is stored in the \l error property.
 */
void UCAlarm::save()
{
    if (d_ptr->status == InProgress) {
        qmlInfo(this) << UbuntuI18n::instance().tr("Alarm has a pending operation.");
        return;
    }

    d_ptr->error = NoError;
    d_ptr->status = Ready;

    if (d_ptr->message().isEmpty()) {
        d_ptr->setMessage(UbuntuI18n::instance().tr("Alarm"));
        d_ptr->changes |= AlarmManager::Message;
    }

    UCAlarm::Error result = d_ptr->checkAlarm();
    if (result != UCAlarm::NoError) {
        d_ptr->_q_syncStatus(Saving, Fail, result);
    } else {
        d_ptr->save();
    }
}

/*!
 * \qmlmethod Alarm::cancel()
 * The function removes the alarm from the collection. The function will fail
 * for alarms which are not yet registered to the collection.
 *
 * The operation is asynchronous, and its status is reported through the \l status
 * property. Further operations should wait till the previous operation is completed.
 * The operation result is stored in the \l error property.
 */
void UCAlarm::cancel()
{
    if (d_ptr->status == InProgress) {
        qmlInfo(this) << UbuntuI18n::instance().tr("Alarm has a pending operation.");
        return;
    }

    d_ptr->error = NoError;
    d_ptr->status = Ready;
    d_ptr->cancel();
}

/*!
 * \qmlmethod Alarm::reset()
 * The function resets the alarm properties to its defaults. After this call the
 * object can be used to create a new alarm event.
 *
 * \b Note: do not call this function on alarm objects retrieved from AlarmModel, as
 * calling it will result in the model being out of sync from the alarm database.
 */
void UCAlarm::reset()
{
    d_ptr->_q_syncStatus(Reseting, InProgress, NoError);

    d_ptr->reset();
    d_ptr->setDefaults();
    d_ptr->changes = AlarmManager::AllFields;
    d_ptr->_q_syncStatus(Reseting, Ready, NoError);
}

/*!
 * \internal
 * The method returns the alarm cookie.
 */
QVariant UCAlarm::cookie() const
{
    return d_ptr->cookie();
}

#include "moc_ucalarm.cpp"
