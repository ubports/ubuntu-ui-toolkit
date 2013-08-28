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
#include "alarmrequest_p.h"
#include "alarmrequest_p_p.h"

#include <qorganizer.h>
#include <qorganizermanager.h>

// FIXME: remove once we have the Organizer backend ready
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>

#define ALARM_DATABASE          "%1/alarms.database"
#define ALARM_MANAGER           "eds"
#define ALARM_MANAGER_FALLBACK  "memory"
#define ALARM_COLLECTION        "Alarms"

QTORGANIZER_USE_NAMESPACE

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms. QOrganizer implementation may not require this,
 * however in case we decide to go with some other approach, this layer is welcome.
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

    void completeUpdate();
    void completeRemove();
    void completeFetch();
};

class AlarmsAdapter : public AlarmManagerPrivate
{
public:
    enum Error {
        DifferentTypeExpected = UCAlarm::AdaptationError + 1,
        FetchedEventEmpty,  // 102
        UnhandledEventType, // 103
        UnhandledRequest,   // 104
        OrganizerError      // 105 onwards
    };

    AlarmsAdapter(AlarmManager *qq);
    virtual ~AlarmsAdapter();

    static AlarmsAdapter* get(AlarmManager *instance = 0) {
        return static_cast<AlarmsAdapter*>(AlarmManagerPrivate::get(instance));
    }

    bool usingCollection: 1;
    QOrganizerManager *manager;
    QOrganizerCollection collection;

    virtual bool fetchAlarms();
    void completeFetchAlarms(const QList<QOrganizerItem> &alarmList);

    // FIXME: remove once we have the Organizer backend ready
    void loadAlarms();
    void saveAlarms();

    void rawAlarm2Organizer(const AlarmData &alarm, QOrganizerTodo &event);
    void updateOrganizerFromRaw(const AlarmData &alarm, QOrganizerTodo &event);
    int organizer2RawAlarm(const QOrganizerItem &item, AlarmData &alarm);
    QSet<Qt::DayOfWeek> daysToSet(const AlarmData &alarm) const;
    void daysFromSet(AlarmData &alarm, QSet<Qt::DayOfWeek> set);
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
    , usingCollection(false)
    , manager(0)
{
    QOrganizerManager local;
    bool usingDefaultManager = local.availableManagers().contains(ALARM_MANAGER);
    manager = (usingDefaultManager) ? new QOrganizerManager(ALARM_MANAGER) : new QOrganizerManager(ALARM_MANAGER_FALLBACK);
    manager->setParent(q_ptr);
    if (!usingDefaultManager) {
        qWarning() << "WARNING: default alarm manager not installed, using" << manager->managerName() << "manager.";
        qWarning() << "This manager may not provide all the needed features.";
    }

    QList<QOrganizerCollection> collections = manager->collections();
    if (collections.count() > 0) {
        Q_FOREACH(const QOrganizerCollection &c, collections) {
            if (c.metaData(QOrganizerCollection::KeyName).toString() == ALARM_COLLECTION) {
                usingCollection = true;
                collection = c;
                break;
            }
        }
    }
    if (!usingCollection) {
        // create alarm collection
        collection.setMetaData(QOrganizerCollection::KeyName, ALARM_COLLECTION);
        if (manager->saveCollection(&collection)) {
            // make sure we have the collection in the list
            collections = manager->collections();
            if (collections.count() > 0) {
                Q_FOREACH(const QOrganizerCollection &c, collections) {
                    if (c.metaData(QOrganizerCollection::KeyName).toString() == ALARM_COLLECTION) {
                        usingCollection = true;
                        collection = c;
                        break;
                    }
                }
            }
            if (!usingCollection) {
                qWarning() << "WARNING: Creating dedicated collection for alarms was not possible, alarms will be saved into the default collection!";
            }
        }
    }
    loadAlarms();
}

AlarmsAdapter::~AlarmsAdapter()
{
    saveAlarms();
}

// load fallback manager data
void AlarmsAdapter::loadAlarms()
{
    if (manager->managerName() == ALARM_MANAGER) {
        return;
    }
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
        manager->saveItem(&event);
    }
    file.close();
}

// save fallback manager data only
void AlarmsAdapter::saveAlarms()
{
    if (manager->managerName() == ALARM_MANAGER) {
        return;
    }
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
    if (usingCollection) {
        event.setCollectionId(collection.id());
    }
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
            rule.setDaysOfWeek(daysToSet(alarm));
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
        event = static_cast<QOrganizerTodo>(manager->item(eventId));
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

    // check if the alarm is enabled or not
    QOrganizerItemVisualReminder visual = event.detail(QOrganizerItemDetail::TypeVisualReminder);
    QOrganizerItemAudibleReminder audible = event.detail(QOrganizerItemDetail::TypeAudibleReminder);
    alarm.enabled = !visual.isEmpty() && !audible.isEmpty();

    // repeating
    QOrganizerRecurrenceRule rule = event.recurrenceRule();
    switch (rule.frequency()) {
    case QOrganizerRecurrenceRule::Invalid: {
        alarm.type = UCAlarm::OneTime;
        alarm.days = UCAlarmPrivate::dayOfWeek(alarm.date);
        break;
    }
    case QOrganizerRecurrenceRule::Daily: {
        alarm.type = UCAlarm::Repeating;
        alarm.days = UCAlarm::Daily;
        break;
    }
    case QOrganizerRecurrenceRule::Weekly: {
        alarm.type = UCAlarm::Repeating;
        daysFromSet(alarm, rule.daysOfWeek());
        break;
    }
    default:
        // fallback to OneTime, no error reported
        return UnhandledEventType;
    }

    return UCAlarm::NoError;
}

QSet<Qt::DayOfWeek> AlarmsAdapter::daysToSet(const AlarmData &alarm) const
{
    QSet<Qt::DayOfWeek> result;
    for (Qt::DayOfWeek day = Qt::Monday; day <= Qt::Sunday; day = static_cast<Qt::DayOfWeek>(static_cast<int>(day) + 1)) {
        if (alarm.days & (1 << (static_cast<int>(day) - 1)))
            result << day;
    }
    return result;
}

void AlarmsAdapter::daysFromSet(AlarmData &alarm, QSet<Qt::DayOfWeek> set)
{
    alarm.days = 0;
    QSetIterator<Qt::DayOfWeek> i(set);
    while (i.hasNext()) {
        int day = static_cast<int>(i.next());
        alarm.days |= static_cast<UCAlarm::DayOfWeek>(1 << (day - 1));
    }
}


/*-----------------------------------------------------------------------------
 * Abstract methods
 */
bool AlarmsAdapter::fetchAlarms()
{
    // create self deleting request
    AlarmRequest *request = new AlarmRequest(true, q_ptr);
    AlarmRequestAdapter *adapter = static_cast<AlarmRequestAdapter*>(AlarmRequestPrivate::get(request));
    return adapter->fetch();
}

void AlarmsAdapter::completeFetchAlarms(const QList<QOrganizerItem> &alarms)
{
    alarmList.clear();

    QSet<QOrganizerItemId> parentId;
    Q_FOREACH(const QOrganizerItem &item, alarms) {
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

    saveAlarms();
    if (completed) {
        Q_EMIT q_ptr->alarmsChanged();
    }
    completed = true;
}

/*-----------------------------------------------------------------------------
 * AlarmRequestAdapter implementation
 */

AlarmRequestPrivate * createAlarmRequest(AlarmRequest *request, bool autoDelete)
{
    return new AlarmRequestAdapter(request, autoDelete);
}

AlarmRequestAdapter::AlarmRequestAdapter(AlarmRequest *parent, bool autoDelete)
    : AlarmRequestPrivate(parent, autoDelete)
    , m_request(0)
{
}

/*
 * Save or update an alarm. Returns false on operation error.
 */
bool AlarmRequestAdapter::save(AlarmData &alarm)
{
    QOrganizerTodo event;

    if (!alarm.cookie.isValid()) {
        // new event
        AlarmsAdapter::get()->rawAlarm2Organizer(alarm, event);
    } else {
        // update existing event
        QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
        event = AlarmsAdapter::get()->manager->item(itemId);
        if (event.isEmpty()) {
            setStatus(AlarmRequest::Fail, UCAlarm::AdaptationError);
            return false;
        }
        AlarmsAdapter::get()->updateOrganizerFromRaw(alarm, event);
    }

    QOrganizerItemSaveRequest *operation = new QOrganizerItemSaveRequest(q_ptr);
    operation->setManager(AlarmsAdapter::get()->manager);
    operation->setItem(event);
    return start(operation);
}

/*
 * Removes an alarm from the collection. Returns false on failure.
 */
bool AlarmRequestAdapter::remove(AlarmData &alarm)
{
    if (!alarm.cookie.isValid()) {
        setStatus(AlarmRequest::Fail, UCAlarm::InvalidEvent);
        return false;
    }

    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    QOrganizerItemRemoveByIdRequest *operation = new QOrganizerItemRemoveByIdRequest(q_ptr);
    operation->setManager(AlarmsAdapter::get()->manager);
    operation->setItemId(itemId);
    return start(operation);
}

/*
 * Waits for event completion. msec zero means wait forever.
 */
bool AlarmRequestAdapter::wait(int msec)
{
    return (m_request) ? m_request->waitForFinished(msec) : true;
}

/*
 * Initiates alarm fetching.
 */
bool AlarmRequestAdapter::fetch()
{
    AlarmManager *manager = static_cast<AlarmManager*>(q_ptr->parent());
    AlarmsAdapter *owner = AlarmsAdapter::get(manager);

    QOrganizerItemFetchRequest *operation = new QOrganizerItemFetchRequest(q_ptr);
    operation->setManager(owner->manager);

    // set sort order
    QOrganizerItemSortOrder sortOrder;
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setDetail(QOrganizerItemDetail::TypeTodoTime, QOrganizerTodoTime::FieldDueDateTime);
    operation->setSorting(QList<QOrganizerItemSortOrder>() << sortOrder);

    // set filter
    if (AlarmsAdapter::get(manager)->usingCollection) {
        QOrganizerItemCollectionFilter filter;
        filter.setCollectionId(owner->collection.id());
        operation->setFilter(filter);
    }

    // start request
    return start(operation);
}

/*
 * Starts the asynchronous operation.
 */
bool AlarmRequestAdapter::start(QOrganizerAbstractRequest *operation)
{
    m_request = operation;
    if (!m_request) {
        return false;
    }
    completed = false;
    // make sure we are in progress state
    setStatus(AlarmRequest::InProgress);
    QObject::connect(m_request, SIGNAL(resultsAvailable()), q_ptr, SLOT(_q_updateProgress()));
    if (m_request->start()) {
        // check if the request got completed without having the slot called (some engines may do that)
        if (!completed && m_request->state() >= QOrganizerAbstractRequest::CanceledState) {
            _q_updateProgress();
        }
        return true;
    }
    return false;
}

/*
 * Update operation progress.
 */
void AlarmRequestAdapter::_q_updateProgress()
{
    completed = true;

    QOrganizerAbstractRequest::State state = m_request->state();
    switch (state) {
    case QOrganizerAbstractRequest::InactiveState: {
        setStatus(AlarmRequest::Ready);
        break;
    }
    case QOrganizerAbstractRequest::ActiveState: {
        setStatus(AlarmRequest::InProgress);
        completed = false;
        break;
    }
    case QOrganizerAbstractRequest::CanceledState: {
        setStatus(AlarmRequest::Fail, AlarmsAdapter::OrganizerError + m_request->error());
        break;
    }
    case QOrganizerAbstractRequest::FinishedState: {
        int code = m_request->error();
        if (code != QOrganizerManager::NoError) {
            setStatus(AlarmRequest::Fail, AlarmsAdapter::OrganizerError + code);
        } else {
            switch (m_request->type()) {
            case QOrganizerAbstractRequest::ItemSaveRequest: {
                completeUpdate();
                break;
            }
            case QOrganizerAbstractRequest::ItemRemoveByIdRequest: {
                completeRemove();
                break;
            }
            case QOrganizerAbstractRequest::ItemFetchRequest: {
                completeFetch();
                break;
            }
            default:
                qWarning() << "Unhandled request:" << m_request->type();
                setStatus(AlarmRequest::Fail, AlarmsAdapter::UnhandledRequest);
                break;
            }

            setStatus(AlarmRequest::Ready);
        }
        break;
    }
    default: {
        qWarning() << "Invalid status" << state;
        setStatus(AlarmRequest::Fail, UCAlarm::InvalidEvent);
        break;
    }
    }

    if (completed) {
        // cleanup request
        delete m_request;
        m_request = 0;

        if (autoDelete) {
            q_ptr->deleteLater();
        }
    }
}

void AlarmRequestAdapter::completeUpdate()
{
    UCAlarm *alarm = qobject_cast<UCAlarm*>(q_ptr->parent());
    if (!alarm) {
        return;
    }
    UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(alarm);
    QOrganizerItemSaveRequest *save = qobject_cast<QOrganizerItemSaveRequest*>(m_request);
    pAlarm->rawData.cookie = QVariant::fromValue<QOrganizerItemId>(save->items()[0].id());
    pAlarm->rawData.changes = AlarmData::NoChange;

    // request for refresh
    AlarmManagerPrivate::get()->fetchAlarms();
}

void AlarmRequestAdapter::completeRemove()
{
    UCAlarm *alarm = qobject_cast<UCAlarm*>(q_ptr->parent());
    if (!alarm) {
        return;
    }
    UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(alarm);
    pAlarm->rawData.cookie = QVariant();
    pAlarm->rawData.changes = AlarmData::NoChange;

    // request for refresh
    AlarmManagerPrivate::get()->fetchAlarms();
}

void AlarmRequestAdapter::completeFetch()
{
    AlarmManager *manager = static_cast<AlarmManager*>(q_ptr->parent());
    AlarmsAdapter *owner = AlarmsAdapter::get(manager);
    QOrganizerItemFetchRequest *fetch = static_cast<QOrganizerItemFetchRequest *>(m_request);
    owner->completeFetchAlarms(fetch->items());
}
