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
#include "alarmrequest_p.h"
#include "alarmsadapter_p.h"
#include <qorganizertodooccurrence.h>

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QDebug>

#define ALARM_DATABASE          "%1/alarms.database"
/*
 * The main alarm manager engine used from Saucy onwards is EDS (Evolution Data
 * Server) based. Any previous release uses the generic "memory" manager engine
 * which does not store alarm data, does not schedule organizer events and does
 * not give visual or audible reminding.
 */
#define ALARM_MANAGER           "eds"
#define ALARM_MANAGER_FALLBACK  "memory"
#define ALARM_COLLECTION        "Alarms"

QTORGANIZER_USE_NAMESPACE

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarms.
 */
AlarmManagerPrivate * createAlarmsAdapter(AlarmManager *alarms)
{
    return new AlarmsAdapter(alarms);
}

AlarmsAdapter::AlarmsAdapter(AlarmManager *qq)
    : QObject(qq)
    , AlarmManagerPrivate(qq)
    , listDirty(false)
    , manager(0)
    , fetchRequest(0)
{
    QString envManager(qgetenv("ALARM_BACKEND"));
    if (!envManager.isEmpty() && QOrganizerManager::availableManagers().contains(envManager)) {
        manager = new QOrganizerManager(envManager);
    } else {
        manager = QOrganizerManager::availableManagers().contains(ALARM_MANAGER) ?
                    new QOrganizerManager(ALARM_MANAGER) : new QOrganizerManager(ALARM_MANAGER_FALLBACK);
    }
    manager->setParent(q_ptr);
    if (manager->managerName() != ALARM_MANAGER) {
        qWarning() << "WARNING: default alarm manager not installed, using" << manager->managerName() << "manager.";
        qWarning() << "This manager may not provide all the needed features.";
    }

    QList<QOrganizerCollection> collections = manager->collections();
    if (collections.count() > 0) {
        Q_FOREACH(const QOrganizerCollection &c, collections) {
            if (c.metaData(QOrganizerCollection::KeyName).toString() == ALARM_COLLECTION) {
                collection = c;
                break;
            }
        }
    }
    if (collection.id().isNull()) {
        // create alarm collection
        collection.setMetaData(QOrganizerCollection::KeyName, ALARM_COLLECTION);
        // EDS requires extra metadata to be set
        collection. setExtendedMetaData("collection-type", "Task List");
        if (!manager->saveCollection(&collection)) {
            qWarning() << "WARNING: Creating dedicated collection for alarms was not possible, alarms will be saved into the default collection!";
            collection = manager->defaultCollection();
        }
    }
    loadAlarms();

    // connect to manager to receive changes
    QObject::connect(manager, SIGNAL(dataChanged()), this, SLOT(fetchAlarms()));
    QObject::connect(manager, SIGNAL(itemsAdded(QList<QOrganizerItemId>)), this, SLOT(fetchAlarms()));
    QObject::connect(manager, SIGNAL(itemsChanged(QList<QOrganizerItemId>)), this, SLOT(fetchAlarms()));
    QObject::connect(manager, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)), this, SLOT(fetchAlarms()));
}

AlarmsAdapter::~AlarmsAdapter()
{
    saveAlarms();
}

// load fallback manager data
void AlarmsAdapter::loadAlarms()
{
    if (manager->managerName() != ALARM_MANAGER_FALLBACK) {
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
        alarm.originalDate = alarm.date = AlarmData::transcodeDate(alarm.date, Qt::LocalTime);
        alarm.type = static_cast<UCAlarm::AlarmType>(type);
        alarm.days = static_cast<UCAlarm::DaysOfWeek>(days);

        QOrganizerTodo event;
        organizerEventFromAlarmData(alarm, event);
        manager->saveItem(&event);
    }
    file.close();
}

// save fallback manager data only
void AlarmsAdapter::saveAlarms()
{
    if ((manager->managerName() != ALARM_MANAGER_FALLBACK) || !listDirty) {
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

    Q_FOREACH(const AlarmData &alarm, alarmList) {
        out << alarm.message
            << AlarmData::transcodeDate(alarm.originalDate, Qt::UTC)
            << alarm.sound
            << alarm.type
            << alarm.days
            << alarm.enabled;
    }
    file.close();
    listDirty = false;
}

void AlarmsAdapter::organizerEventFromAlarmData(const AlarmData &alarm, QOrganizerTodo &event)
{
    event.setCollectionId(collection.id());
    event.setAllDay(false);
    event.setStartDateTime(AlarmData::transcodeDate(alarm.date, Qt::UTC));
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

void AlarmsAdapter::updateOrganizerEventFromAlarmData(const AlarmData &alarm, QOrganizerTodo &event)
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

    organizerEventFromAlarmData(alarm, event);
}

int AlarmsAdapter::alarmDataFromOrganizerEvent(const QOrganizerTodo &event, AlarmData &alarm)
{
    if (event.isEmpty()) {
        return FetchedEventEmpty;
    }

    alarm.cookie = QVariant::fromValue<QOrganizerItemId>(event.id());
    alarm.message = event.displayLabel();
    alarm.date = AlarmData::transcodeDate(event.startDateTime().toUTC(), Qt::LocalTime);
    alarm.sound = QUrl(event.description());
    alarm.originalDate = alarm.date;

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
    if (fetchRequest) {
        // there is already a fetch request ongoing, exit
        return false;
    }
    // create self deleting request
    fetchRequest = new AlarmRequest(true, q_ptr);
    AlarmRequestAdapter *adapter = static_cast<AlarmRequestAdapter*>(AlarmRequestPrivate::get(fetchRequest));
    return adapter->fetch();
}

void AlarmsAdapter::completeFetchAlarms(const QList<QOrganizerItem> &alarms)
{
    alarmList.clear();

    QSet<QOrganizerItemId> parentId;
    QOrganizerTodo event;
    Q_FOREACH(const QOrganizerItem &item, alarms) {
        // repeating alarms may be fetched as occurences, therefore check their parent event
        if (item.type() == QOrganizerItemType::TypeTodoOccurrence) {
            QOrganizerTodoOccurrence occurrence = static_cast<QOrganizerTodoOccurrence>(item);
            QOrganizerItemId eventId = occurrence.parentId();
            if (parentId.contains(eventId)) {
                continue;
            }
            parentId << eventId;
            event = static_cast<QOrganizerTodo>(manager->item(eventId));
        } else if (item.type() == QOrganizerItemType::TypeTodo){
            event = static_cast<QOrganizerTodo>(item);
        } else {
            continue;
        }
        AlarmData alarm;
        if (alarmDataFromOrganizerEvent(event, alarm) == UCAlarm::NoError) {
            adjustAlarmOccurrence(event, alarm);
            alarmList << alarm;
        }
    }

    saveAlarms();
    Q_EMIT q_ptr->alarmsChanged();
    completed = true;
    // do we get segfault because this is missing?
    fetchRequest->deleteLater();
    fetchRequest = 0;
}

void AlarmsAdapter::adjustAlarmOccurrence(const QOrganizerTodo &event, AlarmData &alarm)
{
    if (alarm.type == UCAlarm::OneTime) {
        return;
    }
    // with EDS we need to query the occurrences separately as the fetch reports only the main events
    // with fallback manager this does not reduce the performance and does work teh same way.
    QDateTime currentDate = AlarmData::normalizeDate(QDateTime::currentDateTime());
    if (alarm.date > currentDate) {
        // no need to adjust date, the event occurs in the future
        return;
    }
    QDateTime startDate;
    QDateTime endDate;
    if (alarm.type == UCAlarm::Repeating) {
        // 8 days is enough from the starting date (or current date depending on the start date)
        startDate = (alarm.date > currentDate) ? alarm.date : currentDate;
        endDate = startDate.addDays(8);
    }

    // transcode both dates
    startDate = AlarmData::transcodeDate(startDate, Qt::UTC);
    endDate = AlarmData::transcodeDate(endDate, Qt::UTC);

    QList<QOrganizerItem> occurrences = manager->itemOccurrences(event, startDate, endDate, 10);
    // get the first occurrence and use the date from it
    if ((occurrences.length() > 0) && (occurrences[0].type() == QOrganizerItemType::TypeTodoOccurrence)) {
        // loop till we get a proper future due date
        for (int i = 0; i < occurrences.count(); i++) {
            QOrganizerTodoOccurrence occurrence = static_cast<QOrganizerTodoOccurrence>(occurrences[i]);
            // check if the date is after the current datetime
            // the first occurrence is the one closest to the currentDate, therefore we can safely
            // set that startDate to the alarm
            alarm.date = AlarmData::transcodeDate(occurrence.startDateTime().toUTC(), Qt::LocalTime);
            if (alarm.date > currentDate) {
                // we have the proper date set, leave
                break;
            }
        }
    }
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
        AlarmsAdapter::get()->organizerEventFromAlarmData(alarm, event);
    } else {
        // update existing event
        QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
        event = AlarmsAdapter::get()->manager->item(itemId);
        if (event.isEmpty()) {
            setStatus(AlarmRequest::Saving, AlarmRequest::Fail, UCAlarm::AdaptationError);
            return false;
        }
        AlarmsAdapter::get()->updateOrganizerEventFromAlarmData(alarm, event);
    }

    QOrganizerItemSaveRequest *operation = new QOrganizerItemSaveRequest(q_ptr);
    operation->setManager(AlarmsAdapter::get()->manager);
    operation->setItem(event);
    AlarmsAdapter::get()->listDirty = true;
    return start(operation);
}

/*
 * Removes an alarm from the collection. Returns false on failure.
 */
bool AlarmRequestAdapter::remove(AlarmData &alarm)
{
    if (!alarm.cookie.isValid()) {
        setStatus(AlarmRequest::Canceling, AlarmRequest::Fail, UCAlarm::InvalidEvent);
        return false;
    }

    QOrganizerTodo event;
    AlarmsAdapter::get()->organizerEventFromAlarmData(alarm, event);
    event.setId(alarm.cookie.value<QOrganizerItemId>());

    QOrganizerItemRemoveRequest *operation = new QOrganizerItemRemoveRequest(q_ptr);
    operation->setManager(AlarmsAdapter::get()->manager);
    operation->setItem(event);
    AlarmsAdapter::get()->listDirty = true;
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
    sortOrder.setDetail(QOrganizerItemDetail::TypeTodoTime, QOrganizerTodoTime::FieldStartDateTime);
    operation->setSorting(QList<QOrganizerItemSortOrder>() << sortOrder);

    // set filter
    QOrganizerItemCollectionFilter filter;
    filter.setCollectionId(owner->collection.id());
    operation->setFilter(filter);

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
    setStatus(requestTypeToOperation(), AlarmRequest::InProgress);
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
    AlarmRequest::Operation opCode = requestTypeToOperation();
    switch (state) {
    case QOrganizerAbstractRequest::InactiveState: {
        setStatus(opCode, AlarmRequest::Ready);
        break;
    }
    case QOrganizerAbstractRequest::ActiveState: {
        setStatus(opCode, AlarmRequest::InProgress);
        completed = false;
        break;
    }
    case QOrganizerAbstractRequest::CanceledState: {
        setStatus(opCode, AlarmRequest::Fail, AlarmsAdapter::OrganizerError + m_request->error());
        break;
    }
    case QOrganizerAbstractRequest::FinishedState: {
        int code = m_request->error();
        if (code != QOrganizerManager::NoError) {
            setStatus(opCode, AlarmRequest::Fail, AlarmsAdapter::OrganizerError + code);
        } else {
            switch (opCode) {
            case AlarmRequest::Saving: {
                completeUpdate();
                break;
            }
            case AlarmRequest::Canceling: {
                completeRemove();
                break;
            }
            case AlarmRequest::Fetching: {
                completeFetch();
                break;
            }
            default:
                qWarning() << "Unhandled request:" << m_request->type();
                setStatus(opCode, AlarmRequest::Fail, AlarmsAdapter::UnhandledRequest);
                break;
            }

            setStatus(opCode, AlarmRequest::Ready);
        }
        break;
    }
    default: {
        qWarning() << "Invalid status" << state;
        setStatus(opCode, AlarmRequest::Fail, UCAlarm::InvalidEvent);
        break;
    }
    }

    if (completed) {
        // cleanup request
        m_request->deleteLater();
        m_request = 0;

        if (autoDelete) {
            q_ptr->deleteLater();
        }
    }
}

AlarmRequest::Operation AlarmRequestAdapter::requestTypeToOperation()
{
    switch (m_request->type()) {
    case QOrganizerAbstractRequest::ItemSaveRequest: {
        return AlarmRequest::Saving;
    }
    case QOrganizerAbstractRequest::ItemRemoveRequest: {
        return AlarmRequest::Canceling;
    }
    case QOrganizerAbstractRequest::ItemFetchRequest: {
        return AlarmRequest::Fetching;
    }
    default:
        return AlarmRequest::NoOperation;
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
}

void AlarmRequestAdapter::completeFetch()
{
    AlarmManager *manager = static_cast<AlarmManager*>(q_ptr->parent());
    AlarmsAdapter *owner = AlarmsAdapter::get(manager);
    QOrganizerItemFetchRequest *fetch = static_cast<QOrganizerItemFetchRequest *>(m_request);
    owner->completeFetchAlarms(fetch->items());
}
