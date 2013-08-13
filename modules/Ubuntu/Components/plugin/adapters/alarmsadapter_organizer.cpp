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
#include "ucalarmmanager.h"
#include "ucalarmmanager_p.h"

#include <qorganizer.h>
#include <qorganizermanager.h>

// FIXME: remove once we have the Organizer backend ready
#include <QtCore/QFile>
#include <QtCore/QDir>

#define ALARM_DATABASE      "%1/.config/ubuntu-ui-toolkit/alarms.database"

QTORGANIZER_USE_NAMESPACE

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms. QOrganizer implementation may not require this,
 * however in case we decide to go with some other approach, this layer is welcome.
 */

class AlarmsAdapter : public UCAlarmManagerPrivate {
public:
    AlarmsAdapter(UCAlarmManager *qq);
    virtual ~AlarmsAdapter();

    QOrganizerManager manager;
    int cookieCount;

    virtual QList<RawAlarm> getAlarms();
    virtual bool addAlarm(RawAlarm &alarm);
    virtual bool updateAlarm(RawAlarm &alarm);
    virtual bool removeAlarm(RawAlarm &alarm);

private:
    void rawAlarm2Organizer(const RawAlarm &alarm, QOrganizerTodo &event);
    void updateOrganizerFromRaw(const RawAlarm &alarm, QOrganizerTodo &event);
    bool organizer2RawAlarm(const QOrganizerItem &item, RawAlarm &alarm);
    // FIXME: remove once we have the Organizer backend ready
    void loadAlarms();
    void saveAlarms();
};

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms. QOrganizer implementation may not require this,
 * however in case we decide to go with some other approach, this layer is welcome.
 */
UCAlarmManagerPrivate * createAlarmsAdapter(UCAlarmManager *alarms)
{
    return new AlarmsAdapter(alarms);
}

AlarmsAdapter::AlarmsAdapter(UCAlarmManager *qq)
    : UCAlarmManagerPrivate(qq)
    , manager("memory")
    , cookieCount(0)
{
    loadAlarms();
}

AlarmsAdapter::~AlarmsAdapter()
{
    saveAlarms();
}

// FIXME: remove once we have the Organizer backend ready
void AlarmsAdapter::loadAlarms()
{
    QFile file(QString(ALARM_DATABASE).arg(QDir::homePath()));
    if (!file.open(QFile::ReadOnly)) {
        return;
    }
    QDataStream in(&file);

    while (!in.atEnd()) {
        RawAlarm alarm;

        int type, days;
        in >> alarm.message >> alarm.date >> alarm.tone >> type >> days >> alarm.enabled;
        alarm.type = static_cast<UCAlarm::AlarmType>(type);
        alarm.days = static_cast<UCAlarm::DaysOfWeek>(days);

        QOrganizerTodo event;
        rawAlarm2Organizer(alarm, event);
        manager.saveItem(&event);
    }
    file.close();
}

// FIXME: remove once we have the Organizer backend ready
void AlarmsAdapter::saveAlarms()
{
    QFile file(QString(ALARM_DATABASE).arg(QDir::homePath()));
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }
    QDataStream out(&file);

    Q_FOREACH(UCAlarm *alarm, alarmList) {
        UCAlarmPrivate *palarm = UCAlarmPrivate::get(alarm);
        out << palarm->rawData.message
            << palarm->rawData.date
            << palarm->rawData.tone
            << palarm->rawData.type
            << palarm->rawData.days
            << palarm->rawData.enabled;
    }
    file.close();
}

void AlarmsAdapter::rawAlarm2Organizer(const RawAlarm &alarm, QOrganizerTodo &event)
{
    event.setAllDay(false);
    event.setDueDateTime(alarm.date);
    event.setDisplayLabel(alarm.message);

    if (alarm.enabled) {
        // set visual and audible reminder serving as alarm note
        QOrganizerItemVisualReminder visual;
        visual.setSecondsBeforeStart(0);
        visual.setMessage(alarm.message);
        event.saveDetail(&visual);

        QOrganizerItemAudibleReminder audible;
        audible.setSecondsBeforeStart(0);
        audible.setDataUrl(alarm.tone);
        event.saveDetail(&audible);
    }

    // save the tone as description as the audible reminder may be off
    event.setDescription(alarm.tone.toString());

    // set repeating
    switch (alarm.type) {
    case UCAlarm::OneTime: {
        break;
    }
    case UCAlarm::Repeating: {
        QOrganizerRecurrenceRule rule;
        if (alarm.days == UCAlarm::Daily) {
            rule.setFrequency(QOrganizerRecurrenceRule::Daily);
        } else if (alarm.days) {
            rule.setFrequency(QOrganizerRecurrenceRule::Weekly);
            rule.setDaysOfWeek(alarm.daysToSet());
        }
        event.setRecurrenceRule(rule);
        break;
    }
    default:
        break;
    }
}

void AlarmsAdapter::updateOrganizerFromRaw(const RawAlarm &alarm, QOrganizerTodo &event)
{
    // remove affected details
    if (!alarm.enabled || (alarm.changes & RawAlarm::Enabled)) {
        // remove previously set reminders
        QOrganizerItemVisualReminder visual = event.detail(QOrganizerItemDetail::TypeVisualReminder);
        event.removeDetail(&visual);
        QOrganizerItemAudibleReminder audible = event.detail(QOrganizerItemDetail::TypeAudibleReminder);
        event.removeDetail(&audible);
    }
    if (((alarm.changes & RawAlarm::Type) == RawAlarm::Type)
            || ((alarm.changes & RawAlarm::Days) == RawAlarm::Days)) {
        QOrganizerItemRecurrence old = event.detail(QOrganizerItemDetail::TypeRecurrence);
        event.removeDetail(&old);
    }

    rawAlarm2Organizer(alarm, event);
}

bool AlarmsAdapter::organizer2RawAlarm(const QOrganizerItem &item, RawAlarm &alarm)
{
    if ((item.type() != QOrganizerItemType::TypeTodo) &&
        (item.type() != QOrganizerItemType::TypeTodoOccurrence)){
        error(UCAlarmManager::AdaptationError, QString("Error fetching event. Event type is %1, but expected %2")
              .arg(item.type())
              .arg(QOrganizerItemType::TypeEvent));
        return false;
    }

    QOrganizerTodo event;
    if (item.type() == QOrganizerItemType::TypeTodoOccurrence) {
        QOrganizerTodoOccurrence occurence = static_cast<QOrganizerTodoOccurrence>(item);
        QOrganizerItemId eventId = occurence.parentId();
        event = static_cast<QOrganizerTodo>(manager.item(eventId));
    } else {
        event = static_cast<QOrganizerTodo>(item);
    }
    if (event.isEmpty()) {
        error(UCAlarmManager::AdaptationError, "Fetched alarm event reported to be empty.");
        return false;
    }

    alarm.cookie = QVariant::fromValue<QOrganizerItemId>(event.id());
    alarm.message = event.displayLabel();
    alarm.date = event.dueDateTime();
    alarm.tone = QUrl(event.description());

    // repeating
    QOrganizerRecurrenceRule rule = event.recurrenceRule();
    switch (rule.frequency()) {
    case QOrganizerRecurrenceRule::Invalid: {
        alarm.type = UCAlarm::OneTime;
        alarm.days = dayOfWeek(alarm.date);
        break;
    }
    case QOrganizerRecurrenceRule::Daily: {
        alarm.type = UCAlarm::Repeating;
        alarm.days = UCAlarm::Daily;
        break;
    }
    case QOrganizerRecurrenceRule::Weekly: {
        alarm.type = UCAlarm::Repeating;
        alarm.daysFromSet(rule.daysOfWeek());
        break;
    }
    default:
        error(UCAlarmManager::AdaptationError, QString("Unhandled recurence: %1. Defaulting to OneTime").arg(rule.frequency()));
        alarm.type = UCAlarm::OneTime;
        alarm.days = dayOfWeek(alarm.date);
        break;
    }

    return true;
}

/*-----------------------------------------------------------------------------
 * Abstract methods
 */
QList<RawAlarm> AlarmsAdapter::getAlarms()
{
    QOrganizerItemSortOrder sortOrder;
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setDetail(QOrganizerItemDetail::TypeTodoTime, QOrganizerTodoTime::FieldDueDateTime);

    QList<QOrganizerItem> items = manager.items(QDateTime(), QDateTime(), QOrganizerItemFilter(), 200,
                                                QList<QOrganizerItemSortOrder>() << sortOrder);
    QList<RawAlarm> result;

    QSet<QOrganizerItemId> parentId;
    Q_FOREACH(const QOrganizerItem &item, items) {
        // repeating alarms may be fetched as occurences, therefore check their parent event
        if (item.type() == QOrganizerItemType::TypeTodoOccurrence) {
            QOrganizerTodoOccurrence occurence = static_cast<QOrganizerTodoOccurrence>(item);
            if (parentId.contains(occurence.parentId())) {
                // skip this as we only consider parent items
                continue;
            }
            parentId << occurence.parentId();
        } else if (item.type() != QOrganizerItemType::TypeTodo) {
            // skip any other type than Event
            continue;
        }
        RawAlarm alarm;
        if (organizer2RawAlarm(item, alarm)) {
            result << alarm;
        }
    }

    return result;
}

bool AlarmsAdapter::addAlarm(RawAlarm &alarm)
{
    Q_ASSERT(!alarm.cookie.isValid());

    QOrganizerTodo event;

    rawAlarm2Organizer(alarm, event);

    if (manager.saveItem(&event)) {
        alarm.cookie = QVariant::fromValue<QOrganizerItemId>(event.id());
        alarm.changes = RawAlarm::NoChange;
        refreshAlarms();
        saveAlarms();
        return true;
    }
    error(UCAlarmManager::AdaptationError, QString("Error saving the alarm, code %1").arg(manager.error()));
    return false;
}

bool AlarmsAdapter::updateAlarm(RawAlarm &alarm)
{
    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    QOrganizerTodo event = manager.item(itemId);
    if (event.isEmpty()) {
        error(UCAlarmManager::AdaptationError, "Empty item given to update.");
        return false;
    }

    updateOrganizerFromRaw(alarm, event);
    if (manager.saveItem(&event)) {
        alarm.changes = RawAlarm::NoChange;
        refreshAlarms();
        saveAlarms();
        return true;
    }
    error(UCAlarmManager::AdaptationError, QString("Error updating the alarm, code %1").arg(manager.error()));
    return false;
}

bool AlarmsAdapter::removeAlarm(RawAlarm &alarm)
{
    if (!alarm.cookie.isValid()) {
        error(UCAlarmManager::InvalidEvent, "Removing unregistered alarm");
        return false;
    }

    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    if (manager.removeItem(itemId)) {
        alarm.cookie = QVariant();
        refreshAlarms();
        saveAlarms();
        return true;
    }
    error(UCAlarmManager::AdaptationError, QString("Error on removing alarm, code %1").arg(manager.error()));
    return false;
}
