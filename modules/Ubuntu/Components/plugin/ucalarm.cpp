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
#include "ucalarmmanager_p.h"
#include "i18n.h"

UCAlarmPrivate::UCAlarmPrivate(UCAlarm *qq)
    : q_ptr(qq)
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
 * \brief Alarm component is a representation of an alarm.
 *
 * The Alarm component unifies the properties of an alarm event. The component
 * does not check the consistency of the properties, only holds and groups the
 * values. Both one time and repeating alarm events are supported by the component.
 *
 * The component can be used to hold data in alarm editors as shown in the following
 * code snippet:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * Rectangle {
 *     width: units.gu(40)
 *     height: units.gu(20)
 *
 *     property Alarm alarm: Alarm{
 *         date: new Date()
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
 *                 if (!AlarmManager.set(alarm)) print(AlarmManager.errorMessage)
 *                 alarm.reset();
 *             }
 *         }
 *     }
 * }
 * \endqml
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
    d_ptr->rawData.days = UCAlarmManagerPrivate::dayOfWeek(d_ptr->rawData.date);
}

UCAlarm::UCAlarm(const UCAlarm &other)
    : QObject(other.parent())
    , d_ptr(new UCAlarmPrivate(this))
{
    d_ptr->rawData = other.d_func()->rawData;
}

UCAlarm::UCAlarm(const QDateTime &dt, AlarmType type, DaysOfWeek days, const QString &message, QObject *parent)
    : QObject(parent)
    , d_ptr(new UCAlarmPrivate(this))
{
    d_ptr->rawData.date = dt;
    d_ptr->rawData.type = type;
    d_ptr->rawData.days = days;
    if (!message.isEmpty()) {
        d_ptr->rawData.message = message;
    }
    if (d_ptr->rawData.days == AutoDetect) {
        d_ptr->rawData.days = UCAlarmManagerPrivate::dayOfWeek(d_ptr->rawData.date);
    }
}


UCAlarm::~UCAlarm()
{
}

bool UCAlarm::operator==(const UCAlarm &other)
{
    Q_D(UCAlarm);
    return d->rawData.compare(other.d_func()->rawData);
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
    d->rawData.changes |= RawAlarm::Enabled;
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
    d->rawData.changes |= RawAlarm::Date;
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
    d->rawData.changes |= RawAlarm::Message;
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
    d->rawData.changes |= RawAlarm::Type;
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
 *  \li Alarm.Autodetect
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
    d->rawData.changes |= RawAlarm::Days;
    Q_EMIT daysOfWeekChanged();
}

/*!
 * \qmlproperty url Alarm::tone
 * The property holds the alarm's tone to be played when the alarm is triggered.
 * An empty url will mean to play the default tone.
 *
 * The defaul tvalue is an empty url.
 */
QUrl UCAlarm::tone() const
{
    Q_D(const UCAlarm);
    return d->rawData.tone;
}
void UCAlarm::setTone(const QUrl &tone)
{
    Q_D(UCAlarm);
    if (d->rawData.tone == tone) {
        return;
    }
    d->rawData.tone = tone;
    d->rawData.changes |= RawAlarm::Tone;
    Q_EMIT toneChanged();
}

/*!
 * \qmlmethod Alarm::reset()
 * The function resets the alarm object to its defaults. After this call the
 * object can be used to create a new alarm event.
 */
void UCAlarm::reset()
{
    Q_D(UCAlarm);
    d->rawData = RawAlarm();
    d->setDefaults();
    Q_EMIT enabledChanged();
    Q_EMIT dateChanged();
    Q_EMIT messageChanged();
    Q_EMIT toneChanged();
    Q_EMIT typeChanged();
    Q_EMIT daysOfWeekChanged();
}
