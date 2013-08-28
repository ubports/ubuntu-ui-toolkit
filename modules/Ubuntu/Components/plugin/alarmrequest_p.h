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

#ifndef ALARMREQUEST_P_H
#define ALARMREQUEST_P_H

#include <QObject>
#include "alarmmanager_p.h"

class AlarmRequestPrivate;
class AlarmRequest : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Ready = 1,
        InProgress,
        Fail
    };

    explicit AlarmRequest(QObject *parent = 0);
    AlarmRequest(bool autoDelete, QObject *parent = 0);
    ~AlarmRequest();

    Status status() const;
    int error() const;

    bool save(AlarmData &alarm);
    bool remove(AlarmData &alarm);

Q_SIGNALS:
    void statusChanged(int status, int error);

private:
    Q_DECLARE_PRIVATE(AlarmRequest)
    QScopedPointer<AlarmRequestPrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _q_updateProgress())
};

#endif // ALARMREQUEST_P_H
