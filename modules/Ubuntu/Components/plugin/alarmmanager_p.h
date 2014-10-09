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

#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtCore/QSet>

#include "ucalarm.h"

class AlarmUtils {
public:
    AlarmUtils()
    {
    }

    static QHash<int, QByteArray> roles() {
        QHash<int, QByteArray> hash;
        int i = 0;
        hash.insert(i++, QByteArray("message"));
        hash.insert(i++, QByteArray("date"));
        hash.insert(i++, QByteArray("type"));
        hash.insert(i++, QByteArray("daysOfWeek"));
        hash.insert(i++, QByteArray("sound"));
        hash.insert(i++, QByteArray("enabled"));
        hash.insert(i++, QByteArray("model"));
        return hash;
    }

    static QVariant roleData(int role, UCAlarm *alarm) {
        switch (role) {
        case 0: return alarm->message();
        case 1: return alarm->date();
        case 2: return alarm->type();
        case 3: return static_cast<int>(alarm->daysOfWeek());
        case 4: return alarm->sound();
        case 5: return alarm->enabled();
        default: return QVariant::fromValue(alarm);
        }
    }

    static QDateTime normalizeDate(const QDateTime &dt) {
        QTime time = dt.time();
        time.setHMS(time.hour(), time.minute(), time.second());
        return QDateTime(dt.date(), time, dt.timeSpec());
    }
};

class AlarmRequest;
class AlarmManagerPrivate;
class AlarmRequestPrivate;
class AlarmList;
class AlarmManager : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Ready = 1,
        Progress,
        Error
    };
    enum Change {
        NoChange    = 0,
        Enabled     = 0x0001,
        Date        = 0x0002,
        Message     = 0x0004,
        Sound       = 0x0008,
        Type        = 0x0010,
        Days        = 0x0020,
        AllFields   = 0x00FF
    };

    ~AlarmManager();

    static AlarmManager &instance();

    bool fetchAlarms();
    int alarmCount();
    UCAlarm *alarmAt(int index) const;
    UCAlarm *findAlarm(const QVariant &cookie) const;

    bool verifyChange(UCAlarm *alarm, Change change, const QVariant &newData);
    static UCAlarmPrivate *createAlarmData(UCAlarm *alarm);

Q_SIGNALS:
    void alarmsChanged();
    void alarmUpdated(int index);
    void alarmRemoveStarted(int index);
    void alarmRemoveFinished();

private:
    explicit AlarmManager(QObject *parent = 0);
    Q_DISABLE_COPY(AlarmManager)
//    Q_DECLARE_PRIVATE(AlarmManager)
    friend class AlarmManagerPrivate;
    QScopedPointer<AlarmManagerPrivate> d_ptr;
};

#endif // ALARMMANAGER_H
