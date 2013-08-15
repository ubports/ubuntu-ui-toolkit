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

#ifndef UCALARMSERVICES_P_H
#define UCALARMSERVICES_P_H

#include "ucalarmmanager.h"
#include <QtCore/QUrl>

#include <QDebug>

class RawAlarm {
public:
    enum Change {
        NoChange    = 0,
        Enabled     = 0x0001,
        Date        = 0x0002,
        Message     = 0x0004,
        Sound       = 0x0008,
        Type        = 0x0010,
        Days        = 0x0020
    };

    RawAlarm()
        : changes(0)
        , type(UCAlarm::OneTime)
        , days(UCAlarm::AutoDetect)
        , enabled(true)
    {
    }
    RawAlarm(const RawAlarm &other)
        : changes(0)
        , cookie(other.cookie)
        , date(other.date)
        , message(other.message)
        , type(other.type)
        , days(other.days)
        , enabled(other.enabled)
    {
    }

    inline bool compare(const RawAlarm &other)
    {
        // cookie, sound, and enabled do not count on alarm equality
        return date == other.date
                && message == other.message
                && type == other.type
                && days == other.days;
    }

    inline bool operator==(const RawAlarm &other)
    {
        return compare(other);
    }

    inline QSet<Qt::DayOfWeek> daysToSet() const
    {
        QSet<Qt::DayOfWeek> result;
        for (Qt::DayOfWeek day = Qt::Monday; day <= Qt::Sunday; day = static_cast<Qt::DayOfWeek>(static_cast<int>(day) + 1)) {
            if (days & (1 << (static_cast<int>(day) - 1)))
                result << day;
        }
        return result;
    }

    inline void daysFromSet(QSet<Qt::DayOfWeek> set)
    {
        days = 0;
        QSetIterator<Qt::DayOfWeek> i(set);
        while (i.hasNext()) {
            int day = static_cast<int>(i.next());
            days |= static_cast<UCAlarm::DayOfWeek>(1 << (day - 1));
        }
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

class UCAlarmManagerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(UCAlarmManager)
public:
    UCAlarmManagerPrivate(UCAlarmManager *qq);
    virtual ~UCAlarmManagerPrivate();

    UCAlarmManager *q_ptr;
    QList<UCAlarm*> alarmList;
    bool completed:1;

    void error(UCAlarmManager::Error code, const QString &msg);
    void resetError();

    virtual QList<RawAlarm> getAlarms() = 0;
    virtual bool addAlarm(RawAlarm &alarm) = 0;
    virtual bool updateAlarm(RawAlarm &alarm) = 0;
    virtual bool removeAlarm(RawAlarm &alarm) = 0;

    // utility functions
    static UCAlarm::DayOfWeek dayOfWeek(const QDateTime &dt);
    static int firstDayOfWeek(UCAlarm::DaysOfWeek days);
    static int nextDayOfWeek(UCAlarm::DaysOfWeek days, int fromDay);
    static bool multipleDaysSet(UCAlarm::DaysOfWeek days);

protected Q_SLOTS:
    void refreshAlarms();

private:
    void clear();
    bool checkAlarm(RawAlarm &alarm);
    bool checkDow(RawAlarm &alarm);
    bool checkOneTime(RawAlarm &alarm);
    bool checkRepeatingWeekly(RawAlarm &alarm);

    // property getter
    QQmlListProperty<UCAlarm> alarms();

    UCAlarmManager::Error errorCode;
    QString errorMessage;
};

UCAlarmManagerPrivate * createAlarmsAdapter(UCAlarmManager *alarms);

#endif // UCALARMSERVICES_P_H
