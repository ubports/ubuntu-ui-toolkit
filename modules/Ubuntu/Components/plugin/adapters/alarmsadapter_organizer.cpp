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
#include "alarmmanager_p.h"
#include "alarmmanager_p_p.h"

#include <qorganizer.h>
#include <qorganizermanager.h>

// FIXME: remove once we have the Organizer backend ready
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#define ALARM_DATABASE      "%1/alarms.database"

QTORGANIZER_USE_NAMESPACE

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms. QOrganizer implementation may not require this,
 * however in case we decide to go with some other approach, this layer is welcome.
 */

class AlarmsAdapter : public AlarmManagerPrivate
{
public:
    enum Error {
        DifferentTypeExpected = UCAlarm::AdaptationError + 1,
        FetchedEventEmpty,  // 102
        UnhandledEventType, // 103
        OrganizerError      // 104 onwards
    };

    AlarmsAdapter(AlarmManager *qq);
    virtual ~AlarmsAdapter();

    QOrganizerManager manager;
    QOrganizerCollection collection;
    int cookieCount;

    virtual int addAlarm(AlarmData &alarm);
    virtual int updateAlarm(AlarmData &alarm);
    virtual int removeAlarm(AlarmData &alarm);
protected Q_SLOTS:
    virtual void fetchAlarms();

private:
    void rawAlarm2Organizer(const AlarmData &alarm, QOrganizerTodo &event);
    void updateOrganizerFromRaw(const AlarmData &alarm, QOrganizerTodo &event);
    int organizer2RawAlarm(const QOrganizerItem &item, AlarmData &alarm);
    // FIXME: remove once we have the Organizer backend ready
    void loadAlarms();
    void saveAlarms();
};

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms. QOrganizer implementation may not require this,
 * however in case we decide to go with some other approach, this layer is welcome.
 */
AlarmManagerPrivate * createAlarmsAdapter(AlarmManager *alarms)
{
    return new AlarmsAdapter(alarms);
}

AlarmsAdapter::AlarmsAdapter(AlarmManager *qq)
    : AlarmManagerPrivate(qq)
    , manager("memory")
    , cookieCount(0)
{
    bool alarmCollectionFound = false;
    QList<QOrganizerCollection> collections = manager.collections();
    if (collections.count() > 0) {
        Q_FOREACH(const QOrganizerCollection &c, collections) {
            if (c.metaData(QOrganizerCollection::KeyName).toString() == "alarms") {
                alarmCollectionFound = true;
                collection = c;
                break;
            }
        }
    }
    if (!alarmCollectionFound) {
        // create alarm collection
        collection.setMetaData(QOrganizerCollection::KeyName, "alarms");
        manager.saveCollection(&collection);
    }
    loadAlarms();
}

AlarmsAdapter::~AlarmsAdapter()
{
    saveAlarms();
}

// FIXME: remove once we have the Organizer backend ready
void AlarmsAdapter::loadAlarms()
{
    QFile file(QString(ALARM_DATABASE).arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)));
    if (!file.open(QFile::ReadOnly)) {
        return;
    }
    QDataStream in(&file);

    while (!in.atEnd()) {
        AlarmData alarm;

        int type, days;
        in >> alarm.message >> alarm.date >> alarm.sound >> type >> days >> alarm.enabled;
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
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dir.exists()) {
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }
    QFile file(QString(ALARM_DATABASE).arg(dir.path()));
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }
    QDataStream out(&file);

    Q_FOREACH(AlarmData alarm, alarmList) {
        out << alarm.message
            << alarm.date
            << alarm.sound
            << alarm.type
            << alarm.days
            << alarm.enabled;
    }
    file.close();
}

void AlarmsAdapter::rawAlarm2Organizer(const AlarmData &alarm, QOrganizerTodo &event)
{
    event.setCollectionId(collection.id());
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
        audible.setDataUrl(alarm.sound);
        event.saveDetail(&audible);
    }

    // save the sound as description as the audible reminder may be off
    event.setDescription(alarm.sound.toString());

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

void AlarmsAdapter::updateOrganizerFromRaw(const AlarmData &alarm, QOrganizerTodo &event)
{
    // remove affected details
    if (!alarm.enabled || (alarm.changes & AlarmData::Enabled)) {
        // remove previously set reminders
        QOrganizerItemVisualReminder visual = event.detail(QOrganizerItemDetail::TypeVisualReminder);
        event.removeDetail(&visual);
        QOrganizerItemAudibleReminder audible = event.detail(QOrganizerItemDetail::TypeAudibleReminder);
        event.removeDetail(&audible);
    }
    if (((alarm.changes & AlarmData::Type) == AlarmData::Type)
            || ((alarm.changes & AlarmData::Days) == AlarmData::Days)) {
        QOrganizerItemRecurrence old = event.detail(QOrganizerItemDetail::TypeRecurrence);
        event.removeDetail(&old);
    }

    rawAlarm2Organizer(alarm, event);
}

int AlarmsAdapter::organizer2RawAlarm(const QOrganizerItem &item, AlarmData &alarm)
{
    if ((item.type() != QOrganizerItemType::TypeTodo) &&
        (item.type() != QOrganizerItemType::TypeTodoOccurrence)){
        return DifferentTypeExpected;
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
        return FetchedEventEmpty;
    }

    alarm.cookie = QVariant::fromValue<QOrganizerItemId>(event.id());
    alarm.message = event.displayLabel();
    alarm.date = event.dueDateTime();
    alarm.sound = QUrl(event.description());

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
        // fallback to OneTime, no error reported
        return UnhandledEventType;
    }

    return UCAlarm::NoError;
}

/*-----------------------------------------------------------------------------
 * Abstract methods
 */
void AlarmsAdapter::fetchAlarms()
{
    // clear previous alarms
    alarmList.clear();

    QOrganizerItemSortOrder sortOrder;
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setDetail(QOrganizerItemDetail::TypeTodoTime, QOrganizerTodoTime::FieldDueDateTime);

    QOrganizerItemCollectionFilter filter;
    filter.setCollectionId(collection.id());

    QList<QOrganizerItem> items = manager.items(QDateTime(), QDateTime(), filter, -1,
                                                QList<QOrganizerItemSortOrder>() << sortOrder);
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
        AlarmData alarm;
        if (organizer2RawAlarm(item, alarm) == UCAlarm::NoError) {
            alarmList << alarm;
        }
    }
    if (completed) {
        Q_EMIT q_ptr->alarmsChanged();
    }
}

int AlarmsAdapter::addAlarm(AlarmData &alarm)
{
    Q_ASSERT(!alarm.cookie.isValid());

    QOrganizerTodo event;

    rawAlarm2Organizer(alarm, event);

    if (manager.saveItem(&event)) {
        alarm.cookie = QVariant::fromValue<QOrganizerItemId>(event.id());
        alarm.changes = AlarmData::NoChange;
        fetchAlarms();
        saveAlarms();
        return UCAlarm::NoError;
    }
    return OrganizerError + manager.error();
}

int AlarmsAdapter::updateAlarm(AlarmData &alarm)
{
    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    QOrganizerTodo event = manager.item(itemId);
    if (event.isEmpty()) {
        return UCAlarm::AdaptationError;
    }

    updateOrganizerFromRaw(alarm, event);
    if (manager.saveItem(&event)) {
        alarm.changes = AlarmData::NoChange;
        fetchAlarms();
        saveAlarms();
        return UCAlarm::NoError;
    }
    return OrganizerError + manager.error();
}

int AlarmsAdapter::removeAlarm(AlarmData &alarm)
{
    if (!alarm.cookie.isValid()) {
        return UCAlarm::InvalidEvent;
    }

    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    if (manager.removeItem(itemId)) {
        alarm.cookie = QVariant();
        fetchAlarms();
        saveAlarms();
        return UCAlarm::NoError;
    }
    return OrganizerError + manager.error();
}
