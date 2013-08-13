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
#include "ucalarms_p.h"
#include "i18n.h"

UCAlarmPrivate::UCAlarmPrivate(UCAlarm *qq)
    : q_ptr(qq)
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
 */

/*!
 * \enum UCAlarm::AlarmType
 * This enum specifies the frequiency of an alarm:
 * \value OneTime
 * \value Repeating
 * \value Monthly
 * \value Yearly
 * \value CustomDays
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
    d_ptr->rawData.days = UCAlarmsPrivate::dayOfWeek(d_ptr->rawData.date);
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
        d_ptr->rawData.days = UCAlarmsPrivate::dayOfWeek(d_ptr->rawData.date);
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
 * The property holds the date the alarm will be triggered. When a recurring alarm
 * is triggered, the date will be set to the next time the alarm will be kicked. For
 * non-requrring alarms (type set to OneTime) the date will not be modified.
 *
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
