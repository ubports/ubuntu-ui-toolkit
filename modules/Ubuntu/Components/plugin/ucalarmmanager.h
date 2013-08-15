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

#ifndef UCALARMMANAGER_H
#define UCALARMMANAGER_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>

#include "ucalarm.h"

class UCAlarmManagerPrivate;
class UCAlarmManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString erroMessage READ errorMessage NOTIFY errorChanged)
    Q_PRIVATE_PROPERTY(UCAlarmManager::d_func(), QQmlListProperty<UCAlarm> alarms READ alarms NOTIFY alarmsChanged)
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

    explicit UCAlarmManager(QObject *parent = 0);
    ~UCAlarmManager();

    QList<UCAlarm*> alarms();
    Error error() const;
    QString errorMessage() const;

    // invokables to be also accessible from QML
    Q_INVOKABLE bool set(UCAlarm *alarm);
    Q_INVOKABLE bool cancel(UCAlarm *alarm);

    Q_INVOKABLE bool setOneTime(const QDateTime &date, const QString &message);
    Q_INVOKABLE bool setRepeating(const QDateTime &date, int days, const QString &message);


Q_SIGNALS:
    void alarmsChanged();
    void errorChanged();
    
private:
    Q_DISABLE_COPY(UCAlarmManager)
    Q_DECLARE_PRIVATE(UCAlarmManager)
    QScopedPointer<UCAlarmManagerPrivate> d_ptr;
};

#endif // UCALARMMANAGER_H
