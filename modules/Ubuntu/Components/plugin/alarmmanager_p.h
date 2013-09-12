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

class AlarmData {
public:
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

    AlarmData()
        : changes(0)
        , type(UCAlarm::OneTime)
        , days(UCAlarm::AutoDetect)
        , enabled(true)
    {
    }
    AlarmData(const AlarmData &other)
        : changes(0)
        , cookie(other.cookie)
        , date(other.date)
        , message(other.message)
        , type(other.type)
        , days(other.days)
        , enabled(other.enabled)
    {
    }

    bool compare(const AlarmData &other)
    {
        // cookie, sound, and enabled do not count on alarm equality
        return date == other.date
                && message == other.message
                && type == other.type
                && days == other.days;
    }

    bool operator==(const AlarmData &other)
    {
        return compare(other);
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
        return hash;
    }

    QVariant roleData(int role) const {
        switch (role) {
        case 0: return message;
        case 1: return date;
        case 2: return type;
        case 3: return static_cast<int>(days);
        case 4: return sound;
        case 5: return enabled;
        default: return QVariant();
        }
    }

    static QDateTime normalizeDate(const QDateTime &dt) {
        QTime time = dt.time();
        time.setHMS(time.hour(), time.minute(), time.second());
        return QDateTime(dt.date(), time, dt.timeSpec());
    }

    unsigned int changes;
    QVariant cookie;

    // data members
    QDateTime date;
    QString message;
    QUrl sound;
    UCAlarm::AlarmType type;
    UCAlarm::DaysOfWeek days;
    bool enabled;
};

class AlarmRequest;
class AlarmManagerPrivate;
class AlarmManager : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Ready = 1,
        Progress,
        Error
    };

    ~AlarmManager();

    static AlarmManager &instance();

    QList<AlarmData> alarms() const;

Q_SIGNALS:
    void alarmsChanged();

private:
    explicit AlarmManager(QObject *parent = 0);
    Q_DISABLE_COPY(AlarmManager)
    Q_DECLARE_PRIVATE(AlarmManager)
    QScopedPointer<AlarmManagerPrivate> d_ptr;
};

#endif // ALARMMANAGER_H
