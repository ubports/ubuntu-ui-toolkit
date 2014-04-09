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
#include "alarmrequest_p.h"
#include "i18n.h"
#include <QtQml/QQmlInfo>

UCAlarmPrivate::UCAlarmPrivate(UCAlarm *qq)
    : q_ptr(qq)
    , request(0)
    , error(UCAlarm::NoError)
    , status(UCAlarm::Ready)
{
    setDefaults();
}

UCAlarmPrivate::~UCAlarmPrivate()
{
}

void UCAlarmPrivate::setDefaults()
{
    rawData.date = QDateTime::currentDateTime();
    rawData.message = UbuntuI18n::instance().tr("Alarm");
    rawData.days = dayOfWeek(rawData.date);
}

bool UCAlarmPrivate::createRequest()
{
    if (request) {
        return true;
    }
    request = new AlarmRequest(q_ptr);
    if (!request) {
        return false;
    }
    QObject::connect(request, SIGNAL(statusChanged(int,int,int)),
                     q_ptr, SLOT(_q_syncStatus(int,int,int)));
    return true;
}

void UCAlarmPrivate::_q_syncStatus(int operation, int status, int error) {
    UCAlarm::Status alarmStatus = static_cast<UCAlarm::Status>(status);
    if (this->status != alarmStatus || this->error != error) {
        this->status = alarmStatus;
        this->error = error;

        if (this->status == UCAlarm::Ready) {
            // sync field changes occured during operation
            if (rawData.changes & AlarmData::Enabled)
                Q_EMIT q_ptr->enabledChanged();
            if (rawData.changes & AlarmData::Date)
                Q_EMIT q_ptr->dateChanged();
            if (rawData.changes & AlarmData::Message)
                Q_EMIT q_ptr->messageChanged();
            if (rawData.changes & AlarmData::Sound)
                Q_EMIT q_ptr->soundChanged();
            if (rawData.changes & AlarmData::Type)
                Q_EMIT q_ptr->typeChanged();
            if (rawData.changes & AlarmData::Days)
                Q_EMIT q_ptr->daysOfWeekChanged();
            rawData.changes = 0;
        }

        Q_EMIT q_func()->statusChanged(static_cast<UCAlarm::Operation>(operation));
        Q_EMIT q_func()->errorChanged();
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
        // start from teh beginning of the week
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

UCAlarm::Error UCAlarmPrivate::checkAlarm()
{
    if (rawData.message.isEmpty()) {
        rawData.message = UbuntuI18n::instance().tr("Alarm");
        rawData.changes |= AlarmData::Message;
    }

    if (!rawData.date.isValid()) {
        return UCAlarm::InvalidDate;
    }

    rawData.date = rawData.normalizeDate(rawData.date);

    // check type first as it may alter start day
    if (rawData.type == UCAlarm::OneTime) {
       return checkOneTime();
    } else if (rawData.type == UCAlarm::Repeating) {
        return checkRepeatingWeekly();
    }

    return UCAlarm::NoError;
}

UCAlarm::Error UCAlarmPrivate::checkDow()
{
    if (!rawData.days) {
        return UCAlarm::NoDaysOfWeek;
    } else if (rawData.days == UCAlarm::AutoDetect) {
        rawData.days = dayOfWeek(rawData.date);
        rawData.changes |= AlarmData::Days;
    } else if (rawData.type != UCAlarm::OneTime && rawData.days != UCAlarm::Daily) {
        // this block is valid only for repeating alarms
        int alarmDay = firstDayOfWeek(rawData.days);
        int dayOfWeek = rawData.date.date().dayOfWeek();
        if (alarmDay < dayOfWeek) {
            rawData.date = rawData.date.addDays(7 - dayOfWeek + alarmDay);
            rawData.changes |= AlarmData::Date;
        } else if (alarmDay > dayOfWeek) {
            rawData.date = rawData.date.addDays(alarmDay - dayOfWeek);
            rawData.changes |= AlarmData::Date;
        }
    }
    return UCAlarm::NoError;
}

UCAlarm::Error UCAlarmPrivate::checkOneTime()
{
    // check days, days can be set for only one day in this case
    if (multipleDaysSet(rawData.days)) {
        return UCAlarm::OneTimeOnMoreDays;
    }

    // adjust start date and/or dayOfWeek according to their values
    UCAlarm::Error result = checkDow();
    if (result != UCAlarm::NoError) {
        return result;
    }

    // start date should be later then the current date/time
    if (rawData.date <= AlarmData::normalizeDate(QDateTime::currentDateTime())) {
        return UCAlarm::EarlyDate;
    }
    return UCAlarm::NoError;
}

UCAlarm::Error UCAlarmPrivate::checkRepeatingWeekly()
{
    // start date is adjusted depending on the days value;
    // start date can be set to be the current time, as scheduling will move
    // it to the first occurence.
    UCAlarm::Error result = checkDow();
    if (result != UCAlarm::NoError) {
        return result;
    }

    // move start time to the first occurence if needed
    int dayOfWeek = rawData.date.date().dayOfWeek();
    if (!isDaySet(dayOfWeek, rawData.days) || (rawData.date <= QDateTime::currentDateTime())) {
        // check the next occurence of the alarm
        int nextOccurrence = nextDayOfWeek(rawData.days, dayOfWeek);
        if (nextOccurrence == dayOfWeek) {
            // move the date to the same day next week
            rawData.date = rawData.date.addDays(7);
        } else if (nextOccurrence < dayOfWeek) {
             // the starting date should be moved to the next week's occurrence
            rawData.date = rawData.date.addDays(7 - dayOfWeek + nextOccurrence);
        } else {
            // the starting date is still this week
            rawData.date = rawData.date.addDays(nextOccurrence - dayOfWeek);
        }
        rawData.changes |= AlarmData::Date;
    }

    return UCAlarm::NoError;
}


/*!
 * \qmltype Alarm
 * \instantiates UCAlarm
 * \inqmlmodule Ubuntu.Components 0.1
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
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
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
 */

UCAlarm::UCAlarm(QObject *parent)
    : QObject(parent)
    , d_ptr(new UCAlarmPrivate(this))
{
}

UCAlarm::UCAlarm(const QDateTime &dt, const QString &message, QObject *parent)
    : QObject(parent)
    , d_ptr(new UCAlarmPrivate(this))
{
    d_ptr->rawData.date = AlarmData::normalizeDate(dt);
    if (!message.isEmpty()) {
        d_ptr->rawData.message = message;
    }
    d_ptr->rawData.days = UCAlarmPrivate::dayOfWeek(d_ptr->rawData.date);
}

UCAlarm::UCAlarm(const QDateTime &dt, DaysOfWeek days, const QString &message, QObject *parent)
    : QObject(parent)
    , d_ptr(new UCAlarmPrivate(this))
{
    d_ptr->rawData.date = AlarmData::normalizeDate(dt);
    d_ptr->rawData.type = Repeating;
    d_ptr->rawData.days = days;
    if (!message.isEmpty()) {
        d_ptr->rawData.message = message;
    }
    if (d_ptr->rawData.days == AutoDetect) {
        d_ptr->rawData.days = UCAlarmPrivate::dayOfWeek(d_ptr->rawData.date);
    }
}

UCAlarm::~UCAlarm()
{
}

/*!
 * \qmlproperty bool Alarm::enabled
 * The property specifies whether the alarm is enabled or not. Disable dalarms
 * are not scheduled.
 * The default value is true;
 */
bool UCAlarm::enabled() const
{
    Q_D(const UCAlarm);
    return d->rawData.enabled;
}
void UCAlarm::setEnabled(bool enabled)
{
    Q_D(UCAlarm);
    if (d->rawData.enabled == enabled) {
        return;
    }
    d->rawData.enabled = enabled;
    d->rawData.changes |= AlarmData::Enabled;
    Q_EMIT enabledChanged();
}

/*!
 * \qmlproperty Date Alarm::date
 * The property holds the date the alarm will be triggered. The default
 * value is the current date and time the alarm object was created. Further
 * \l reset calls will bring the value back to the time the \l reset was called.
 */
QDateTime UCAlarm::date() const
{
    Q_D(const UCAlarm);
    return d->rawData.date;
}
void UCAlarm::setDate(const QDateTime &date)
{
    Q_D(UCAlarm);
    if (d->rawData.date == AlarmData::normalizeDate(date)) {
        return;
    }
    d->rawData.date = AlarmData::normalizeDate(date);
    d->rawData.changes |= AlarmData::Date;
    Q_EMIT dateChanged();
    if (d->rawData.type == UCAlarm::OneTime) {
        // adjust dayOfWeek as well
        setDaysOfWeek(UCAlarm::AutoDetect);
    }
}

/*!
 * \qmlproperty string Alarm::message
 * The property holds the message string which will be displayed when the alarm
 * is triggered. The default value is the localized "Alarm" text.
 */
QString UCAlarm::message() const
{
    Q_D(const UCAlarm);
    return d->rawData.message;
}
void UCAlarm::setMessage(const QString &message)
{
    Q_D(UCAlarm);
    if (d->rawData.message == message) {
        return;
    }
    d->rawData.message = message;
    d->rawData.changes |= AlarmData::Message;
    Q_EMIT messageChanged();
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
    Q_D(const UCAlarm);
    return d->rawData.type;
}
void UCAlarm::setType(UCAlarm::AlarmType type)
{
    Q_D(UCAlarm);
    if (d->rawData.type == type) {
        return;
    }
    d->rawData.type = type;
    d->rawData.changes |= AlarmData::Type;
    Q_EMIT typeChanged();
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
 *  \li The alarm day will be detected from the alarm date.
 * \endtable
 *
 * The default value is Alarm.AutoDetect.
 */
UCAlarm::DaysOfWeek UCAlarm::daysOfWeek() const
{
    Q_D(const UCAlarm);
    return d->rawData.days;
}
void UCAlarm::setDaysOfWeek(UCAlarm::DaysOfWeek days)
{
    Q_D(UCAlarm);
    if (d->rawData.days == days) {
        return;
    }
    d->rawData.days = days;
    d->rawData.changes |= AlarmData::Days;
    Q_EMIT daysOfWeekChanged();
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
    Q_D(const UCAlarm);
    return d->rawData.sound;
}
void UCAlarm::setSound(const QUrl &sound)
{
    Q_D(UCAlarm);
    if (d->rawData.sound == sound) {
        return;
    }
    d->rawData.sound = sound;
    d->rawData.changes |= AlarmData::Sound;
    Q_EMIT soundChanged();
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
    Q_D(const UCAlarm);
    return d->error;
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
    Q_D(const UCAlarm);
    return d->status;
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
    Q_D(UCAlarm);
    if (d->status == InProgress) {
        qmlInfo(this) << UbuntuI18n::instance().tr("Alarm has a pending operation.");
        return;
    }

    d->error = NoError;
    d->status = Ready;

    UCAlarm::Error result = d->checkAlarm();
    if (result != UCAlarm::NoError) {
        d->_q_syncStatus(Saving, Fail, result);
    } else {
        // the alarm has been modified, therefore update the original date as well
        d->rawData.originalDate = d->rawData.date;
        if (d->createRequest()) {
            d->request->save(d->rawData);
        }
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
    Q_D(UCAlarm);
    if (d->status == InProgress) {
        qmlInfo(this) << UbuntuI18n::instance().tr("Alarm has a pending operation.");
        return;
    }

    d->error = NoError;
    d->status = Ready;
    if (d->createRequest()) {
        d->request->remove(d->rawData);
    }
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
    Q_D(UCAlarm);
    d->_q_syncStatus(Reseting, InProgress, NoError);

    d->rawData = AlarmData();
    d->setDefaults();
    d->rawData.changes = AlarmData::AllFields;
    d->_q_syncStatus(Reseting, Ready, NoError);
}

#include "moc_ucalarm.cpp"
