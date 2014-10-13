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

#ifndef ALARMSADAPTER_P_H
#define ALARMSADAPTER_P_H

#include "alarmmanager_p_p.h"
#include "alarmrequest_p_p.h"

#include <qorganizer.h>
#include <qorganizermanager.h>
#include <qorganizertodo.h>

QTORGANIZER_USE_NAMESPACE

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms.
 */

class AlarmRequestAdapter : public AlarmRequestPrivate
{
public:
    AlarmRequestAdapter(AlarmRequest *parent, bool autoDelete);

    // adaptation methods
    bool save(AlarmData &alarm);
    bool remove(AlarmData &alarm);
    bool wait(int msec);
    bool fetch();

    bool start(QOrganizerAbstractRequest *operation);

    void _q_updateProgress();

private:
    QOrganizerAbstractRequest *m_request;

    AlarmRequest::Operation requestTypeToOperation();
    void completeUpdate();
    void completeRemove();
    void completeFetch();
};

class AlarmsAdapter : public QObject, public AlarmManagerPrivate
{
    Q_OBJECT
public:
    enum Error {
        FetchedEventEmpty = UCAlarm::AdaptationError + 1, // 101
        UnhandledEventType, // 102
        UnhandledRequest,   // 103
        OrganizerError      // 104 onwards
    };

    AlarmsAdapter(AlarmManager *qq);
    virtual ~AlarmsAdapter();

    static AlarmsAdapter* get(AlarmManager *instance = 0) {
        return static_cast<AlarmsAdapter*>(AlarmManagerPrivate::get(instance));
    }

    bool listDirty:1;
    QOrganizerManager *manager;
    QOrganizerCollection collection;

    void completeFetchAlarms(const QList<QOrganizerItem> &alarmList);
    void adjustAlarmOccurrence(const QOrganizerTodo &event, AlarmData &alarm);

    void loadAlarms();
    void saveAlarms();

    void organizerEventFromAlarmData(const AlarmData &alarm, QOrganizerTodo &event);
    int alarmDataFromOrganizerEvent(const QOrganizerTodo &event, AlarmData &alarm);
    QSet<Qt::DayOfWeek> daysToSet(int days) const;
    void daysFromSet(AlarmData &alarm, QSet<Qt::DayOfWeek> set);

    bool verifyChange(const QVariant &cookie, AlarmData::Change change, const QVariant &value);
    bool compareCookies(const QVariant &cookie1, const QVariant &cookie2);

public Q_SLOTS:
    bool fetchAlarms();
    void updateAlarms(const QList<QOrganizerItemId> &list);

protected:
    AlarmRequest *fetchRequest;
};


#endif // ALARMSADAPTER_P_H
