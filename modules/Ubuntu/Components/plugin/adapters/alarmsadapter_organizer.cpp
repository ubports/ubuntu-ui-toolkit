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

class AlarmRequestAdapter : public AlarmRequest
{
public:
    AlarmRequestAdapter(bool autoDelete = false, QObject *parent = 0);

    QOrganizerAbstractRequest *createOperation(QOrganizerAbstractRequest::RequestType type, QOrganizerManager *manager);

    void start();

protected Q_SLOTS:
    void updateProgress();

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

    static AlarmsAdapter* get() {
        return static_cast<AlarmsAdapter*>(AlarmManagerPrivate::get());
    }

    QOrganizerManager manager;
    QOrganizerCollection collection;
    int cookieCount;

    virtual void addAlarm(AlarmRequest *request, AlarmData &alarm);
    virtual void updateAlarm(AlarmRequest *request, AlarmData &alarm);
    virtual void removeAlarm(AlarmRequest *request, AlarmData &alarm);
    virtual void fetchAlarms();

    void completeFetchAlarms(const QList<QOrganizerItem> &alarmList);

    // FIXME: remove once we have the Organizer backend ready
    void loadAlarms();
    void saveAlarms();

private:
    void rawAlarm2Organizer(const AlarmData &alarm, QOrganizerTodo &event);
    void updateOrganizerFromRaw(const AlarmData &alarm, QOrganizerTodo &event);
    int organizer2RawAlarm(const QOrganizerItem &item, AlarmData &alarm);

    friend class AlarmRequestAdapter;
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

    // check if the alarm is enabled or not
    QOrganizerItemVisualReminder visual = event.detail(QOrganizerItemDetail::TypeVisualReminder);
    QOrganizerItemAudibleReminder audible = event.detail(QOrganizerItemDetail::TypeAudibleReminder);
    alarm.enabled = !visual.isEmpty() && !audible.isEmpty();

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
    // create self deleting request
    AlarmRequestAdapter *request = static_cast<AlarmRequestAdapter*>(createAlarmRequest(this, true));
    QOrganizerItemFetchRequest *operation = static_cast<QOrganizerItemFetchRequest *>(
                request->createOperation(QOrganizerAbstractRequest::ItemFetchRequest, &manager));

    // set sort order
    QOrganizerItemSortOrder sortOrder;
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setDetail(QOrganizerItemDetail::TypeTodoTime, QOrganizerTodoTime::FieldDueDateTime);
    operation->setSorting(QList<QOrganizerItemSortOrder>() << sortOrder);

    // set filter
    QOrganizerItemCollectionFilter filter;
    filter.setCollectionId(collection.id());
    operation->setFilter(filter);

    // start request
    request->start();
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


void AlarmsAdapter::addAlarm(AlarmRequest *request, AlarmData &alarm)
{
    Q_ASSERT(!alarm.cookie.isValid());

    QOrganizerTodo event;

    rawAlarm2Organizer(alarm, event);

    AlarmRequestAdapter *prequest = static_cast<AlarmRequestAdapter*>(request);
    QOrganizerItemSaveRequest *operation = static_cast<QOrganizerItemSaveRequest *>(
                prequest->createOperation(QOrganizerAbstractRequest::ItemSaveRequest, &manager));
    // set the request befor e invoked
    operation->setItem(event);
    request->start();
}

void AlarmsAdapter::updateAlarm(AlarmRequest *request, AlarmData &alarm)
{
    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    QOrganizerTodo event = manager.item(itemId);
    if (event.isEmpty()) {
        setRequestStatus(request, AlarmRequest::Fail, UCAlarm::AdaptationError);
        return;
    }

    updateOrganizerFromRaw(alarm, event);
    AlarmRequestAdapter *prequest = static_cast<AlarmRequestAdapter*>(request);
    QOrganizerItemSaveRequest *operation = static_cast<QOrganizerItemSaveRequest *>(
                prequest->createOperation(QOrganizerAbstractRequest::ItemSaveRequest, &manager));
    operation->setItem(event);
    request->start();
}

void AlarmsAdapter::removeAlarm(AlarmRequest *request, AlarmData &alarm)
{
    if (!alarm.cookie.isValid()) {
        setRequestStatus(request, AlarmRequest::Fail, UCAlarm::InvalidEvent);
        return;
    }

    QOrganizerItemId itemId = alarm.cookie.value<QOrganizerItemId>();
    AlarmRequestAdapter *prequest = static_cast<AlarmRequestAdapter*>(request);
    QOrganizerItemRemoveByIdRequest *operation = static_cast<QOrganizerItemRemoveByIdRequest *>(
                prequest->createOperation(QOrganizerAbstractRequest::ItemRemoveByIdRequest, &manager));
    operation->setItemId(itemId);
    request->start();
}

AlarmRequest * createAlarmRequest(QObject *owner, bool autoDelete)
{
    return new AlarmRequestAdapter(autoDelete, owner);
}

AlarmRequestAdapter::AlarmRequestAdapter(bool autoDelete, QObject *parent)
    : AlarmRequest(autoDelete, parent)
{
}

QOrganizerAbstractRequest *AlarmRequestAdapter::createOperation(QOrganizerAbstractRequest::RequestType type, QOrganizerManager *manager)
{
    switch (type) {
    case QOrganizerAbstractRequest::ItemSaveRequest: {
        m_request = new QOrganizerItemSaveRequest(this);
        break;
    }
    case QOrganizerAbstractRequest::ItemRemoveByIdRequest: {
        m_request = new QOrganizerItemRemoveByIdRequest(this);
        break;
    }
    case QOrganizerAbstractRequest::ItemFetchRequest: {
        m_request = new QOrganizerItemFetchRequest(this);
        break;
    }
    default:
        m_request = 0;
        break;
    }

    if (m_request) {
        QObject::connect(m_request, SIGNAL(resultsAvailable()), this, SLOT(updateProgress()));
        m_request->setManager(manager);
    }
    return m_request;
}

void AlarmRequestAdapter::start()
{
    if (!m_request) {
        return;
    }
    m_completed = false;
    // make sure we are in progress state
    setStatus(InProgress);
    if (m_request->start()) {
        // check if the request got completed without having the slot called (some engines may do that)
        if (!m_completed && m_request->state() >= QOrganizerAbstractRequest::CanceledState) {
            updateProgress();
        }
    }
}

void AlarmRequestAdapter::updateProgress()
{
    m_completed = true;

    QOrganizerAbstractRequest::State state = m_request->state();
    switch (state) {
    case QOrganizerAbstractRequest::InactiveState: {
        AlarmManagerPrivate::setRequestStatus(this, Ready);
        break;
    }
    case QOrganizerAbstractRequest::ActiveState: {
        setStatus(InProgress);
        m_completed = false;
        break;
    }
    case QOrganizerAbstractRequest::CanceledState: {
        setStatus(Fail, AlarmsAdapter::OrganizerError + m_request->error());
        break;
    }
    case QOrganizerAbstractRequest::FinishedState: {
        int code = m_request->error();
        if (code != QOrganizerManager::NoError) {
            setStatus(Fail, AlarmsAdapter::OrganizerError + code);
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
                qDebug() << "Unhandled request:" << m_request->type();
                setStatus(Fail, AlarmsAdapter::UnhandledRequest);
                break;
            }

            setStatus(Ready);
        }
        break;
    }
    default: {
        qDebug() << "Invalid status" << state;
        setStatus(Fail, UCAlarm::InvalidEvent);
        break;
    }
    }

    if (m_completed) {
        // cleanup request
        delete m_request;
        m_request = 0;

        if (m_autoDelete) {
            deleteLater();
        }
    }
}

void AlarmRequestAdapter::completeUpdate()
{
    UCAlarm *alarm = qobject_cast<UCAlarm*>(parent());
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
    UCAlarm *alarm = qobject_cast<UCAlarm*>(parent());
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
    AlarmsAdapter *owner = static_cast<AlarmsAdapter*>(parent());
    QOrganizerItemFetchRequest *fetch = static_cast<QOrganizerItemFetchRequest *>(m_request);
    owner->completeFetchAlarms(fetch->items());
}
