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

#ifndef ALARMREQUEST_P_P_H
#define ALARMREQUEST_P_P_H

#include "alarmrequest_p.h"

class AlarmRequestPrivate {
    Q_DECLARE_PUBLIC(AlarmRequest)
public:
    AlarmRequestPrivate(AlarmRequest *qq, bool autoDelete);
    virtual ~AlarmRequestPrivate();

    AlarmRequest *q_ptr;
    AlarmRequest::Status status;
    int error;
    bool completed:1;
    bool autoDelete:1;

    static AlarmRequestPrivate *get(AlarmRequest *instance) {
        return instance->d_func();
    }

    void setStatus(AlarmRequest::Operation operation, AlarmRequest::Status status, int error = UCAlarm::NoError);

    virtual bool save(AlarmData &alarm) = 0;
    virtual bool remove(AlarmData &alarm) = 0;
    virtual bool wait(int msec) = 0;
    virtual void _q_updateProgress() = 0;
};

AlarmRequestPrivate * createAlarmRequest(AlarmRequest *request, bool autoDelete);

#endif // ALARMREQUEST_P_P_H
