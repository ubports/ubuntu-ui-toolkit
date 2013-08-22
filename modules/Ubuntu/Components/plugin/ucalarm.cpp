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
#include "alarmmanager_p_p.h"
#include "i18n.h"

UCAlarmPrivate::UCAlarmPrivate(UCAlarm *qq)
    : q_ptr(qq)
    , error(UCAlarm::NoError)
{
    setDefaults();
}

void UCAlarmPrivate::setDefaults()
{
    rawData.date = QDateTime::currentDateTime();
    rawData.message = UbuntuI18n::instance().tr("Alarm");
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
 *                 else alarm.reset();
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
    d_ptr->rawData.date = dt;
    if (!message.isEmpty()) {
        d_ptr->rawData.message = message;
    }
    d_ptr->rawData.days = AlarmManagerPrivate::dayOfWeek(d_ptr->rawData.date);
}

UCAlarm::UCAlarm(const QDateTime &dt, DaysOfWeek days, const QString &message, QObject *parent)
    : QObject(parent)
    , d_ptr(new UCAlarmPrivate(this))
{
    d_ptr->rawData.date = dt;
    d_ptr->rawData.type = Repeating;
    d_ptr->rawData.days = days;
    if (!message.isEmpty()) {
        d_ptr->rawData.message = message;
    }
    if (d_ptr->rawData.days == AutoDetect) {
        d_ptr->rawData.days = AlarmManagerPrivate::dayOfWeek(d_ptr->rawData.date);
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
    if (d->rawData.date == date) {
        return;
    }
    d->rawData.date = date;
    d->rawData.changes |= AlarmData::Date;
    Q_EMIT dateChanged();
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
 * The operation result is stored in the \l error property.
 */
void UCAlarm::save()
{
    Q_D(UCAlarm);
    int changes = 0;
    d->error = AlarmManager::instance().set(d->rawData, changes);
    if (d->error != NoError) {
        Q_EMIT errorChanged();
    } else {
        if (changes & AlarmData::Enabled)
            Q_EMIT enabledChanged();
        if (changes & AlarmData::Date)
            Q_EMIT dateChanged();
        if (changes & AlarmData::Message)
            Q_EMIT messageChanged();
        if (changes & AlarmData::Sound)
            Q_EMIT soundChanged();
        if (changes & AlarmData::Type)
            Q_EMIT typeChanged();
        if (changes & AlarmData::Days)
            Q_EMIT daysOfWeekChanged();
    }
}

/*!
 * \qmlmethod Alarm::cancel()
 * The function removes the alarm from the collection. The function will fail
 * for alarms which are not yet registered to the collection.
 *
 * The operation result is stored in the \l error property.
 */
void UCAlarm::cancel()
{
    Q_D(UCAlarm);
    d->error = AlarmManager::instance().cancel(d->rawData);
    if (d->error != NoError) {
        Q_EMIT errorChanged();
    }
}

/*!
 * \qmlmethod Alarm::reset()
 * The function resets the alarm properties to its defaults. After this call the
 * object can be used to create a new alarm event.
 */
void UCAlarm::reset()
{
    Q_D(UCAlarm);
    d->error = NoError;
    d->rawData = AlarmData();
    d->setDefaults();
    Q_EMIT enabledChanged();
    Q_EMIT dateChanged();
    Q_EMIT messageChanged();
    Q_EMIT soundChanged();
    Q_EMIT typeChanged();
    Q_EMIT daysOfWeekChanged();
}
