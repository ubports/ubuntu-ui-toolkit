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

#ifndef UCALARMS_H
#define UCALARMS_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>

#include "ucalarm.h"

class UCAlarmsPrivate;
class UCAlarms : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PRIVATE_PROPERTY(UCAlarms::d_func(), QQmlListProperty<UCAlarm> alarms READ alarms NOTIFY alarmsChanged)
public:
    enum Error {
        NoError = 0,
        InvalidDate = -1,
        EarlyDate = -2,
        NoDaysOfWeek = -3,
        OneTimeOnMoreDays = -4,
        InvalidEvent = -5,
        AdaptationError = -100
    };

    explicit UCAlarms(QObject *parent = 0);
    ~UCAlarms();

    QList<UCAlarm*> alarms();
    Error error() const;
    QString errorMessage() const;

    // invokables to be also accessible from QML
    Q_INVOKABLE bool set(UCAlarm *alarm);
    Q_INVOKABLE bool cancel(UCAlarm *alarm);

    Q_INVOKABLE bool set(const QDateTime &date, const QString &message = QString());
    Q_INVOKABLE bool setRepeating(const QDateTime &date, UCAlarm::DaysOfWeek days, const QString &message = QString());


Q_SIGNALS:
    void alarmsChanged();
    void errorChanged();
    
private:
    Q_DISABLE_COPY(UCAlarms)
    Q_DECLARE_PRIVATE(UCAlarms)
    QScopedPointer<UCAlarmsPrivate> d_ptr;
};

#endif // UCALARMS_H
