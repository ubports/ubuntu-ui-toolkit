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

#include "ucalarm_p.h"
#include "ucalarm_p_p.h"
#include "alarmmanager_p_p.h"
#include "alarmsadapter_p.h"
#include <qorganizertodooccurrence.h>

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTimeZone>
#include <QtCore/QStandardPaths>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>

#define ALARM_DATABASE          "%1/alarms.json"
/*
 * The main alarm manager engine used from Saucy onwards is EDS (Evolution Data
 * Server) based. Any previous release uses the generic "memory" manager engine
 * which does not store alarm data, does not schedule organizer events and does
 * not give visual or audible reminding.
 */
#define ALARM_MANAGER           "eds"
#define ALARM_MANAGER_FALLBACK  "memory"
#define ALARM_COLLECTION        "Alarms"

// special tags used as workaround for bug #1361702
const char *tagAlarmService = "x-canonical-alarm";
const char *tagDisabledAlarm = "x-canonical-disabled";

QTORGANIZER_USE_NAMESPACE

UT_NAMESPACE_BEGIN

/*-----------------------------------------------------------------------------
 * Adaptation layer for Alarm Data.
 */
AlarmDataAdapter::AlarmDataAdapter(UCAlarm *qq)
    : UCAlarmPrivate(qq)
{
    reset();
}

AlarmDataAdapter::~AlarmDataAdapter()
{
}

bool AlarmDataAdapter::enabled() const
{
    return !event.tags().contains(tagDisabledAlarm);
}
bool AlarmDataAdapter::setEnabled(bool enabled)
{
    if (enabled == this->enabled()) {
        return false;
    }
    if (enabled) {
        // remove eventual x-canonical-disabled tag as agreed in bug #1361702
        QStringList tags = event.tags();
        tags.removeAll(tagDisabledAlarm);
        event.setTags(tags);
    } else {
        // tag the alarm as disabled, using x-canonical-disabled as agreed in bug #1361702
        event.addTag(tagDisabledAlarm);
    }
    return true;
}

QDateTime AlarmDataAdapter::date() const
{
    return QDateTime(event.startDateTime().date(), event.startDateTime().time(), Qt::LocalTime);
}
bool AlarmDataAdapter::setDate(const QDateTime &date)
{
    if (this->date() == date) {
        return false;
    }
    QDateTime dt = AlarmUtils::normalizeDate(date);
    if (AlarmsAdapter::get()->manager->managerName() == ALARM_MANAGER) {
        // use invalid timezone to simulate floating time for EDS backend
        dt = QDateTime(dt.date(), dt.time(), QTimeZone());
    }
    event.setStartDateTime(dt);
    return true;
}

QString AlarmDataAdapter::message() const
{
    return event.displayLabel();
}
bool AlarmDataAdapter::setMessage(const QString &message)
{
    if (event.displayLabel() == message) {
        return false;
    }
    event.setDisplayLabel(message);
    // set the visual attachment
    QOrganizerItemVisualReminder visual = event.detail(QOrganizerItemDetail::TypeVisualReminder);
    if (!visual.isEmpty()) {
        // remove previous attachment
        event.removeDetail(&visual);
    }
    visual.setSecondsBeforeStart(0);
    visual.setMessage(message);
    event.saveDetail(&visual);
    return true;
}

UCAlarm::AlarmType AlarmDataAdapter::type() const
{
    return alarmType;
}
bool AlarmDataAdapter::setType(UCAlarm::AlarmType type)
{
    if (alarmType == type) {
        return false;
    }
    alarmType = type;
    return true;
}

UCAlarm::DaysOfWeek AlarmDataAdapter::daysOfWeek() const
{
    return dow;
}
bool AlarmDataAdapter::setDaysOfWeek(UCAlarm::DaysOfWeek days)
{
    if (days == dow) {
        return false;
    }
    dow = days;
    return true;
}

QUrl AlarmDataAdapter::sound() const
{
    QOrganizerItemAudibleReminder audible = event.detail(QOrganizerItemDetail::TypeAudibleReminder);
    return audible.dataUrl();
}
bool AlarmDataAdapter::setSound(const QUrl &sound)
{
    QOrganizerItemAudibleReminder audible = event.detail(QOrganizerItemDetail::TypeAudibleReminder);
    if (audible.dataUrl() == sound) {
        return false;
    }

    // make sure we have the attachments saved in any case
    if (!audible.isEmpty()) {
        // remove previous attachment
        event.removeDetail(&audible);
    }
    audible.setSecondsBeforeStart(0);
    audible.setDataUrl(sound);
    event.saveDetail(&audible);
    return true;
}

QVariant AlarmDataAdapter::cookie() const
{
    return QVariant::fromValue(event.id());
}

UCAlarm::Error AlarmDataAdapter::checkAlarm()
{
    QDateTime dt = date();
    if (!dt.isValid()) {
        return UCAlarm::InvalidDate;
    }

    setDate(AlarmUtils::normalizeDate(dt));

    // check type first as it may alter start day
    UCAlarm::Error result = UCAlarm::NoError;
    UCAlarm::AlarmType alarmType = type();
    if (alarmType == UCAlarm::OneTime) {
       result = checkOneTime();
    } else if (alarmType == UCAlarm::Repeating) {
       result = checkRepeatingWeekly();
    }

    if (result == UCAlarm::NoError) {
        // update recurrence
        adjustDowSettings(alarmType, dow);
    }

    return result;
}
// create a request and push the job
/*
 * Save or update an alarm. Returns false on operation error.
 */
void AlarmDataAdapter::save()
{
    // if we have an inactive request, delete it
    if (request && !request->isActive()) {
        delete request;
    }
    if (event.id().managerUri().isEmpty()) {
        changes = AlarmManager::AllFields;
    }
    QOrganizerItemSaveRequest *saveRequest = new QOrganizerItemSaveRequest(q_ptr);
    saveRequest->setItem(event);
    request = saveRequest;
    startOperation(UCAlarm::Saving, SLOT(completeSave()));
}

void AlarmDataAdapter::cancel()
{
    // if we have an inactive request, delete it
    if (request && !request->isActive()) {
        delete request;
    }
    if (event.id().managerUri().isEmpty()) {
        _q_syncStatus(UCAlarm::Canceling, UCAlarm::Fail, UCAlarm::InvalidEvent);
        return;
    }
    QOrganizerItemRemoveByIdRequest *cancelRequest = new QOrganizerItemRemoveByIdRequest(q_ptr);
    cancelRequest->setItemId(event.id());
    request = cancelRequest;
    startOperation(UCAlarm::Canceling, SLOT(completeCancel()));
}

void AlarmDataAdapter::reset()
{
    setData(QOrganizerTodo());
}

// starts an outstanding organizer operation
void AlarmDataAdapter::startOperation(UCAlarm::Operation operation, const char *completionSlot)
{
    request->setManager(AlarmsAdapter::get()->manager);
    _q_syncStatus(operation, UCAlarm::InProgress, UCAlarm::NoError);
    QObject::connect(request.data(), SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), q_ptr, completionSlot);
    if (!request->start()) {
        _q_syncStatus(operation, UCAlarm::Fail, AlarmsAdapter::UnhandledRequest);
    }
}

// complete save operation; new alarms will be fetched when itemsModified() signal is received.
void AlarmDataAdapter::completeSave()
{
    // we either get error or success
    if (request->state() != QOrganizerAbstractRequest::FinishedState) {
        return;
    }
    int error = request->error();
    if (error != QOrganizerManager::NoError) {
        _q_syncStatus(UCAlarm::Saving, UCAlarm::Fail, AlarmsAdapter::OrganizerError + error);
    } else {
        // complete save
        QOrganizerItemSaveRequest *save = static_cast<QOrganizerItemSaveRequest*>(request.data());
        if (save->items().count() <= 0) {
            _q_syncStatus(UCAlarm::Saving, UCAlarm::Fail, UCAlarm::InvalidEvent);
        } else {
            setData(save->items()[0]);
            changes = AlarmManager::NoChange;
            _q_syncStatus(UCAlarm::Saving, UCAlarm::Ready, UCAlarm::NoError);
        }
    }
}

void AlarmDataAdapter::completeCancel()
{
    // we either get error or success
    if (request->state() != QOrganizerAbstractRequest::FinishedState) {
        return;
    }
    int error = request->error();
    if (error != QOrganizerManager::NoError) {
        _q_syncStatus(UCAlarm::Canceling, UCAlarm::Fail, AlarmsAdapter::OrganizerError + error);
    } else {
        // complete save
        setData(QOrganizerTodo());
        changes = AlarmManager::NoChange;
        _q_syncStatus(UCAlarm::Canceling, UCAlarm::Ready, UCAlarm::NoError);
    }
}

void AlarmDataAdapter::copyAlarmData(const UCAlarm &other)
{
    AlarmDataAdapter *pOther = static_cast<AlarmDataAdapter*>(AlarmDataAdapter::get(&other));
    setData(pOther->data());
}

void AlarmDataAdapter::adjustDowSettings(UCAlarm::AlarmType type, UCAlarm::DaysOfWeek days)
{
    QOrganizerItemRecurrence old = event.detail(QOrganizerItemDetail::TypeRecurrence);
    event.removeDetail(&old);

    switch (type) {
    case UCAlarm::OneTime: {
        break;
    }
    case UCAlarm::Repeating: {
        QOrganizerRecurrenceRule rule;
        if (days == UCAlarm::AutoDetect) {
            rule.setFrequency(QOrganizerRecurrenceRule::Weekly);
            rule.setDaysOfWeek(daysToSet(dayOfWeek(date())));
        } else if (days == UCAlarm::Daily) {
            rule.setFrequency(QOrganizerRecurrenceRule::Daily);
        } else if (days) {
            rule.setFrequency(QOrganizerRecurrenceRule::Weekly);
            rule.setDaysOfWeek(daysToSet(days));
        }
        event.setRecurrenceRule(rule);
        break;
    }
    default:
        break;
    }
}

QSet<Qt::DayOfWeek> AlarmDataAdapter::daysToSet(int days)
{
    QSet<Qt::DayOfWeek> result;
    for (Qt::DayOfWeek day = Qt::Monday; day <= Qt::Sunday; day = static_cast<Qt::DayOfWeek>(static_cast<int>(day) + 1)) {
        if (days & (1 << (static_cast<int>(day) - 1)))
            result << day;
    }
    return result;
}

UCAlarm::DaysOfWeek AlarmDataAdapter::daysFromSet(const QSet<Qt::DayOfWeek> &set)
{
    UCAlarm::DaysOfWeek days = 0;
    QSetIterator<Qt::DayOfWeek> i(set);
    while (i.hasNext()) {
        int day = static_cast<int>(i.next());
        days |= static_cast<UCAlarm::DayOfWeek>(1 << (day - 1));
    }
    return days;
}

void AlarmDataAdapter::setData(const QOrganizerTodo &data)
{
    event = data;
    // add x-canonical-alarm tag as agreed in bug #1361702
    if (!event.tags().contains(tagAlarmService)) {
        event.addTag(tagAlarmService);
    }
    event.setCollectionId(AlarmsAdapter::get()->collection.id());
    event.setAllDay(false);
    QOrganizerRecurrenceRule rule = event.recurrenceRule();
    switch (rule.frequency()) {
    case QOrganizerRecurrenceRule::Weekly: {
        alarmType = UCAlarm::Repeating;
        dow = daysFromSet(rule.daysOfWeek());
        break;
    }
    case QOrganizerRecurrenceRule::Daily: {
        alarmType = UCAlarm::Repeating;
        dow = UCAlarm::Daily;
        break;
    }
    default:
        alarmType = UCAlarm::OneTime;
        dow = dayOfWeek(date());
        break;
    }
}

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
    , manager(0)
{
    // register QOrganizerItemId comparators so QVariant == operator can compare them
    QMetaType::registerComparators<QOrganizerItemId>();

    QString envManager(qgetenv("ALARM_BACKEND"));
    if (envManager.isEmpty())
        envManager = ALARM_MANAGER;
    if (!QOrganizerManager::availableManagers().contains(envManager)) {
        qWarning() << "WARNING: alarm manager" << envManager << "not installed, using" << QString(ALARM_MANAGER_FALLBACK);
        envManager = ALARM_MANAGER_FALLBACK;
    }
    manager = new QOrganizerManager(envManager);
    manager->setParent(q_ptr);

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
}

void AlarmsAdapter::alarmOperation(QList<QPair<QOrganizerItemId,QOrganizerManager::Operation> > list)
{
    typedef QPair<QOrganizerItemId,QOrganizerManager::Operation> OperationPair;
    Q_FOREACH(const OperationPair &op, list) {
        switch (op.second) {
        case QOrganizerManager::Add: {
            insertAlarm(op.first);
            break;
        }
        case QOrganizerManager::Change: {
            updateAlarm(op.first);
            break;
        }
        case QOrganizerManager::Remove: {
            removeAlarm(op.first);
            // save alarm data
            break;
        }
        }
    }
    saveAlarms();
}

void AlarmsAdapter::init()
{
    completed = true;
    loadAlarms();

    // connect to manager to receive changes
    QObject::connect(manager, SIGNAL(itemsModified(QList<QPair<QOrganizerItemId,QOrganizerManager::Operation> >)),
                     this, SLOT(alarmOperation(QList<QPair<QOrganizerItemId,QOrganizerManager::Operation> >)));
    QObject::connect(manager, SIGNAL(dataChanged()), this, SLOT(fetchAlarms()));
}

AlarmsAdapter::~AlarmsAdapter()
{
}

UCAlarmPrivate * AlarmsAdapter::createAlarmData(UCAlarm *alarm)
{
    return new AlarmDataAdapter(alarm);
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
    QByteArray data = file.readAll();
    QJsonDocument document(QJsonDocument::fromJson(data));
    QJsonArray array = document.array();
    for (int i = 0; i < array.size(); i++) {
        QJsonObject object = array[i].toObject();

        // use UCAlarm to save store JSON data
        UCAlarm alarm;
        alarm.setMessage(object["message"].toString());
        alarm.setDate(QDateTime::fromString(object["date"].toString()));
        alarm.setSound(object["sound"].toString());
        alarm.setType(static_cast<UCAlarm::AlarmType>(object["type"].toInt()));
        alarm.setDaysOfWeek(static_cast<UCAlarm::DaysOfWeek>(object["days"].toInt()));
        alarm.setEnabled(object["enabled"].toBool());

        AlarmDataAdapter *pAlarm = static_cast<AlarmDataAdapter*>(UCAlarmPrivate::get(&alarm));
        // call checkAlarm to complete field checks (i.e. type vs daysOfWeek, kick date, etc)
        pAlarm->checkAlarm();
        QOrganizerTodo event = pAlarm->data();
        manager->saveItem(&event);
    }
    file.close();
}

// save fallback manager data only
void AlarmsAdapter::saveAlarms()
{
    if (manager->managerName() != ALARM_MANAGER_FALLBACK) {
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
    QJsonArray data;
    for(int i = 0; i < alarmList.count(); i++) {
        // create an UCAlarm and set its event to ease conversions
        const UCAlarm *alarm = alarmList[i];
        QJsonObject object;
        object["message"] = alarm->message();
        object["date"] = alarm->date().toString();
        object["sound"] = alarm->sound().toString();
        object["type"] = QJsonValue(alarm->type());
        object["days"] = QJsonValue(alarm->daysOfWeek());
        object["enabled"] = QJsonValue(alarm->enabled());
        data.append(object);

    }
    QJsonDocument document(data);
    file.write(document.toJson());
    file.close();
}

/*-----------------------------------------------------------------------------
 * Abstract methods
 * verify the adaptation layer for the stored data
 * expected value type based on change:
 * - AlarmData::Enabled - bool
 * - AlarmData::Date - QDateTime
 * - AlarmData::Message - QString
 * - AlarmData::Sound - QString
 * - AlarmData::Type - UCAlarm::Type
 * - AlarmData::Days - UCAlarm::DaysOfWeek
 */
bool AlarmsAdapter::verifyChange(UCAlarm *alarm, AlarmManager::Change change, const QVariant &value)
{
    AlarmDataAdapter *pAlarm = static_cast<AlarmDataAdapter*>(UCAlarmPrivate::get(alarm));
    QOrganizerItemId id = pAlarm->data().id();
    QOrganizerTodo todo = static_cast<QOrganizerTodo>(manager->item(id));
    if (todo.isEmpty()) {
        return false;
    }

    switch (change) {
        case AlarmManager::Enabled:
        {
            Q_ASSERT(value.type() == QVariant::Bool);
            if (value.toBool()) {
                return !todo.tags().contains(tagDisabledAlarm) && todo.tags().contains(tagAlarmService);
            } else {
                return todo.tags().contains(tagDisabledAlarm) && todo.tags().contains(tagAlarmService);
            }
        }
        case AlarmManager::Date:
        {
            return todo.startDateTime() == value.toDateTime();
        }
        case AlarmManager::Message:
        {
            return todo.displayLabel() == value.toString();
        }
        case AlarmManager::Sound:
        {
            // it is enough to check the audible presence, as they are added/removed in pair with visual reminder
            QOrganizerItemAudibleReminder audible = todo.detail(QOrganizerItemDetail::TypeAudibleReminder);
            return audible.dataUrl().toString() == value.toString();
        }
        case AlarmManager::Type:
        {
            QOrganizerRecurrenceRule rule = todo.recurrenceRule();
            QOrganizerRecurrenceRule::Frequency frequency = rule.frequency();
            UCAlarm::AlarmType type = static_cast<UCAlarm::AlarmType>(value.toInt());
            return (type == UCAlarm::OneTime && frequency == QOrganizerRecurrenceRule::Invalid) ||
                    (type == UCAlarm::Repeating &&
                            (frequency == QOrganizerRecurrenceRule::Daily ||
                             frequency == QOrganizerRecurrenceRule::Weekly));
        }
        case AlarmManager::Days:
        {
            // this only checks the days set
            QOrganizerRecurrenceRule rule = todo.recurrenceRule();
            QOrganizerRecurrenceRule::Frequency frequency = rule.frequency();
            if (frequency == QOrganizerRecurrenceRule::Invalid) {
                return false;
            }
            int days = value.toInt();
            if (days == UCAlarm::Daily && frequency == QOrganizerRecurrenceRule::Daily) {
                return true;
            }
            if (frequency == QOrganizerRecurrenceRule::Weekly) {
                return rule.daysOfWeek() == AlarmDataAdapter::daysToSet(days);
            }
            return false;
        }
    default:
        return false;
        break;
    }
}

bool AlarmsAdapter::fetchAlarms()
{
    if (fetchRequest && fetchRequest->isActive()) {
        // there is already a fetch request ongoing, exit
        return false;
    }

    if (!fetchRequest) {
        fetchRequest = new QOrganizerItemFetchRequest(this);
        fetchRequest->setManager(manager);

        // set sort order
        QOrganizerItemSortOrder sortOrder;
        sortOrder.setDirection(Qt::AscendingOrder);
        sortOrder.setDetail(QOrganizerItemDetail::TypeTodoTime, QOrganizerTodoTime::FieldStartDateTime);
        fetchRequest->setSorting(QList<QOrganizerItemSortOrder>() << sortOrder);

        // set filter
        QOrganizerItemCollectionFilter filter;
        filter.setCollectionId(collection.id());
        fetchRequest->setFilter(filter);

        // start request
        QObject::connect(fetchRequest, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(completeFetchAlarms()));
    }

    Q_EMIT q_ptr->alarmsRefreshStarted();
    return fetchRequest->start();
}

int AlarmsAdapter::alarmCount()
{
    return alarmList.count();
}

UCAlarm *AlarmsAdapter::getAlarmAt(int index) const
{
    Q_ASSERT(index >= 0 && index < alarmList.count());
    return const_cast<UCAlarm*>(alarmList[index]);
}

bool AlarmsAdapter::findAlarm(const UCAlarm &alarm, const QVariant &cookie) const
{
    QOrganizerItemId id = cookie.value<QOrganizerItemId>();
    QOrganizerItem item = manager->item(id);
    if (item.type() == QOrganizerItemType::TypeTodo) {
        AlarmDataAdapter *pAlarm = static_cast<AlarmDataAdapter*>(UCAlarmPrivate::get(&alarm));
        pAlarm->setData(static_cast<QOrganizerTodo>(item));
        return true;
    }
    return false;
}

// returns a todo event from an ID, which can be an occurence
QOrganizerTodo AlarmsAdapter::todoItem(const QOrganizerItemId &id)
{
    QOrganizerTodo event;
    if (!id.isNull()) {
        const QOrganizerItem item = manager->item(id);
        if (item.type() == QOrganizerItemType::TypeTodoOccurrence) {
            QOrganizerTodoOccurrence occurrence = static_cast<QOrganizerTodoOccurrence>(item);
            QOrganizerItemId eventId = occurrence.parentId();
            event = static_cast<QOrganizerTodo>(manager->item(eventId));
        } else if (item.type() == QOrganizerItemType::TypeTodo){
            event = static_cast<QOrganizerTodo>(item);
        }
    }
    return event;
}

void AlarmsAdapter::insertAlarm(const QOrganizerItemId &id)
{
    QOrganizerTodo event = todoItem(id);
    if (event.isEmpty()) {
        return;
    }
    // if we have the alarm registered, leave
    if (alarmList.indexOf(event.id()) >= 0) {
        return;
    }
    // use UCAlarm to fix date
    UCAlarm alarm;
    AlarmDataAdapter *pAlarm = static_cast<AlarmDataAdapter*>(UCAlarmPrivate::get(&alarm));
    pAlarm->setData(event);
    adjustAlarmOccurrence(*pAlarm);

    // insert and get the index
    int index = alarmList.insert(alarm);
    Q_EMIT q_ptr->alarmInsertStarted(index);
    Q_EMIT q_ptr->alarmInsertFinished();
}

// updates an alarm and returns the index, -1 on error
void AlarmsAdapter::updateAlarm(const QOrganizerItemId &id)
{
    QOrganizerTodo event = todoItem(id);
    if (event.isEmpty()) {
        return;
    }
    // update alarm data
    int index = alarmList.indexOf(event.id());
    if (index < 0) {
        // it can be that the organizer item ID is not an alarm or it is an occurrence of
        // an organizer event
        return;
    }
    // use UCAlarm to ease conversions
    UCAlarm alarm;
    AlarmDataAdapter *pAlarm = static_cast<AlarmDataAdapter*>(UCAlarmPrivate::get(&alarm));
    pAlarm->setData(event);
    adjustAlarmOccurrence(*pAlarm);
    int newIndex = alarmList.update(index, alarm);
    if (newIndex == index) {
        Q_EMIT q_ptr->alarmUpdated(index);
    } else {
        Q_EMIT q_ptr->alarmMoveStarted(index, newIndex);
        Q_EMIT q_ptr->alarmMoveFinished();
    }
}

// removes an alarm from the list
void AlarmsAdapter::removeAlarm(const QOrganizerItemId &id)
{
    if (id.isNull()) {
        return;
    }
    int index = alarmList.indexOf(id);
    if (index < 0) {
        // this may be an item we don't handle, organizer manager may report us
        // other calendar event removals as well.
        return;
    }
    // emit removal start
    Q_EMIT q_ptr->alarmRemoveStarted(index);
    alarmList.removeAt(index);
    Q_EMIT q_ptr->alarmRemoveFinished();
}

void AlarmsAdapter::completeFetchAlarms()
{
    if (fetchRequest->state() != QOrganizerAbstractRequest::FinishedState) {
        return;
    }
    alarmList.clear();

    QSet<QOrganizerItemId> parentId;
    QOrganizerTodo event;
    Q_FOREACH(const QOrganizerItem &item, fetchRequest->items()) {
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

        // use UCAlarm to ease conversions
        UCAlarm alarm;
        AlarmDataAdapter *pAlarm = static_cast<AlarmDataAdapter*>(UCAlarmPrivate::get(&alarm));
        pAlarm->setData(event);
        adjustAlarmOccurrence(*pAlarm);
        alarmList.insert(alarm);
    }

    completed = true;
    Q_EMIT q_ptr->alarmsRefreshed();
}

void AlarmsAdapter::adjustAlarmOccurrence(AlarmDataAdapter &alarm)
{
    if (alarm.type() == UCAlarm::OneTime) {
        return;
    }
    // with EDS we need to query the occurrences separately as the fetch reports only the main events
    // with fallback manager this does not reduce the performance and does work the same way.
    QDateTime currentDate = AlarmUtils::normalizeDate(QDateTime::currentDateTime());
    if (alarm.date() > currentDate) {
        // no need to adjust date, the event occurs in the future
        return;
    }
    QDateTime startDate;
    QDateTime endDate;
    if (alarm.type() == UCAlarm::Repeating) {
        // 8 days is enough from the starting date (or current date depending on the start date)
        startDate = (alarm.date() > currentDate) ? alarm.date() : currentDate;
        endDate = startDate.addDays(8);
    }

    QList<QOrganizerItem> occurrences = manager->itemOccurrences(alarm.data(), startDate, endDate, 10);
    // get the first occurrence and use the date from it
    if ((occurrences.length() > 0) && (occurrences[0].type() == QOrganizerItemType::TypeTodoOccurrence)) {
        // loop till we get a proper future due date
        for (int i = 0; i < occurrences.count(); i++) {
            QOrganizerTodoOccurrence occurrence = static_cast<QOrganizerTodoOccurrence>(occurrences[i]);
            // check if the date is after the current datetime
            // the first occurrence is the one closest to the currentDate, therefore we can safely
            // set that startDate to the alarm
            alarm.setDate(occurrence.startDateTime());
            if (alarm.date() > currentDate) {
                // we have the proper date set, leave
                break;
            }
        }
    }
}

UT_NAMESPACE_END
