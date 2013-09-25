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

#include "alarmrequest_p.h"
#include "alarmrequest_p_p.h"

/*-----------------------------------------------------------------------------
 * Alarm request public interface
 */
AlarmRequestPrivate::AlarmRequestPrivate(AlarmRequest *qq, bool autoDelete)
    : q_ptr(qq)
    , status(AlarmRequest::Ready)
    , error(UCAlarm::NoError)
    , completed(true)
    , autoDelete(autoDelete)
{
}

AlarmRequestPrivate::~AlarmRequestPrivate()
{
}

void AlarmRequestPrivate::setStatus(AlarmRequest::Operation operation, AlarmRequest::Status status, int error)
{
    if ((this->status != status) || (this->error != error)) {
        this->status = status;
        this->error = error;
        Q_EMIT q_func()->statusChanged(operation, status, error);
    }
}

AlarmRequest::AlarmRequest(QObject *parent)
    : QObject(parent)
    , d_ptr(createAlarmRequest(this, false))
{
}

AlarmRequest::AlarmRequest(bool autoDelete, QObject *parent)
    : QObject(parent)
    , d_ptr(createAlarmRequest(this, autoDelete))
{
}
AlarmRequest::~AlarmRequest()
{
}

AlarmRequest::Status AlarmRequest::status() const {
    return d_func()->status;
}
int AlarmRequest::error() const {
    return d_func()->error;
}

bool AlarmRequest::save(AlarmData &alarm)
{
    return d_func()->save(alarm);
}

bool AlarmRequest::remove(AlarmData &alarm)
{
    return d_func()->remove(alarm);
}

bool AlarmRequest::wait(int msec)
{
    return d_func()->wait(msec);
}

#include "moc_alarmrequest_p.cpp"
