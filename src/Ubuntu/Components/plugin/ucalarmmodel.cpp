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

#include "ucalarmmodel.h"
#include "ucalarm.h"
#include "ucalarm_p.h"
#include "alarmmanager_p.h"
#include <QtQml/QQmlPropertyMap>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>

/*!
 * \qmltype AlarmModel
 * \instantiates UCAlarmModel
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu-services
 * \brief AlarmModel holds the list of alarms defined.
 *
 * The AlarmModel is a simple container of \l Alarm definitions stored in the alarm
 * collection. The data provided by the model are read only, adding, modifying or
 * removing data is only possible through \l Alarm functions. Any modification on
 * the alarms or any new alarm added to the collection will refresh all the model
 * instances. This also means that the delegates visualizing the model elements will
 * also be re-created.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 * import Ubuntu.Components.ListItems 1.0
 * ListView {
 *     model: AlarmModel {}
 *     width: units.gu(80)
 *     height: units.gu(100)
 *     delegate: Subtitled {
 *         text: message
 *         subText: Qt.formatDateTime(date)
 *     }
 * }
 * \endqml
 *
 * The model defines the same roles as the \l Alarm properties. Note that the use
 * of \a enabled role in delegates may be ambiguous. Therefore the model defines
 * the additional role called \a model, which represents the \l Alarm object from
 * the index the delegate is showing the data, so the \a enabled role can then be
 * accessed by simply dereferencing the model i.e. model.enabled.
 *
 * Example:
 * \qml
 * ListView {
 *     model: AlarmModel{}
 *     delegate: Standard {
 *         text: message
 *         control: Switch {
 *             checked: model.enabled
 *         }
 *     }
 * }
 * \endqml
 *
 * The number of alarm events can be obtained from the \l count property. To get
 * a specific alarm event data from the model, use the \l get() function.
 *
 * As the \l get() function returns the alarm object from the given index. Combined
 * with the alarm functions the alarms can be updated in place.
 *
 * Example:
 * \qml
 * ListView {
 *     model: AlarmModel{
 *         id: alarmModel
 *     }
 *     delegate: Standard {
 *         text: message
 *         control: Switch {
 *             checked: model.enabled
 *             onCheckedChanged: {
 *                 if (checked != model.enabled) {
 *                     var data = alarmModel.get(index);
 *                     data.enabled = checked;
 *                     data.save();
 *                 }
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * In this kind of "in place" updates using the \a model role would give the same result:
 * \qml
 * ListView {
 *     model: AlarmModel{
 *         id: alarmModel
 *     }
 *     delegate: Standard {
 *         text: message
 *         control: Switch {
 *             checked: model.enabled
 *             onCheckedChanged: {
 *                 if (checked != model.enabled) {
 *                     model.enabled = checked;
 *                     model.save();
 *                 }
 *             }
 *         }
 *     }
 * }
 * \endqml
 */

UCAlarmModel::UCAlarmModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_moved(false)
{
    // keep in sync with alarms collection changes
    // some of the connections can be asynchronous, others synchronous
    connect(&AlarmManager::instance(), SIGNAL(alarmsRefreshStarted()), this, SLOT(refreshStart()), Qt::DirectConnection);
    connect(&AlarmManager::instance(), SIGNAL(alarmsRefreshed()), this, SLOT(refreshEnd()), Qt::DirectConnection);
    // get individual alarm data updates
    connect(&AlarmManager::instance(), SIGNAL(alarmUpdated(int)), this, SLOT(update(int)), Qt::DirectConnection);
    // get individual alarm insertion
    connect(&AlarmManager::instance(), SIGNAL(alarmInsertStarted(int)), this, SLOT(insertStarted(int)), Qt::DirectConnection);
    connect(&AlarmManager::instance(), SIGNAL(alarmInsertFinished()), this, SLOT(insertFinished()), Qt::DirectConnection);
    // get individual alarm removal, must be direct
    connect(&AlarmManager::instance(), SIGNAL(alarmRemoveStarted(int)), this, SLOT(removeStarted(int)), Qt::DirectConnection);
    connect(&AlarmManager::instance(), SIGNAL(alarmRemoveFinished()), this, SLOT(removeFinished()), Qt::DirectConnection);
    // get individual alamr move, must be direct
    connect(&AlarmManager::instance(), SIGNAL(alarmMoveStarted(int,int)), this, SLOT(moveStarted(int,int)), Qt::DirectConnection);
    connect(&AlarmManager::instance(), SIGNAL(alarmMoveFinished()), this, SLOT(moveFinished()), Qt::DirectConnection);
}
UCAlarmModel::~UCAlarmModel()
{
}

void UCAlarmModel::classBegin()
{
}
void UCAlarmModel::componentComplete()
{
    // fetch alarms
    refresh();
}

int UCAlarmModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return count();
}
QVariant UCAlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int idx = index.row();
    if ((idx >= AlarmManager::instance().alarmCount()) || (idx < 0)) {
        return QVariant();
    }

    UCAlarm *alarm = AlarmManager::instance().alarmAt(idx);
    return AlarmUtils::roleData(role, alarm);
}

QHash<int, QByteArray> UCAlarmModel::roleNames() const
{
    return AlarmUtils::roles();
}

/*!
 * \qmlmethod Alarm AlarmModel::get(int index)
 * Returns the reference of the alarm event at \a index in the model. This allows the
 * alarm data to be modified and updated either through normal component binding
 * or in Javascript functions.
 *
 * \code
 * Component.onCompleted: {
 *     var alarm = alarmModel.get(0);
 *     alarm.message += " updated";
 *     alarm.save();
 * }
 * \endcode
 *
 * This Javascript code returns the alarm properties from the first index of the
 * model, updates its message and updates the alarm. Note that the alarm must be
 * saved in order to have the changes visible. The follwoing code will not update
 * the alarm in the model/collection:
 *
 * \code
 * Component.onCompleted: {
 *     alarmModel.get(0).message += " updated";
 *     alarm.message += " updated";
 * }
 * \endcode
 *
 * \note The returned object is not guarantied to remain valid, it should not be
 * used in property bindings. Also, \l {Alarm::reset}{reset()} should not be called
 * either as the call will clear the alarm data from the cache.
 *
 * \sa Alarm
 */
UCAlarm* UCAlarmModel::get(int index)
{
    UCAlarm *alarm = AlarmManager::instance().alarmAt(index);
    if (alarm) {
        UCAlarm *tempAlarm = new UCAlarm(this);
        UCAlarmPrivate::get(tempAlarm)->copyAlarmData(*alarm);
        alarm = tempAlarm;
        QQmlEngine::setObjectOwnership(tempAlarm, QQmlEngine::JavaScriptOwnership);
    }
    return alarm;
}

/*!
 * \qmlproperty int AlarmModel::count
 * The number of data entries in the model.
 */
int UCAlarmModel::count() const
{
    return AlarmManager::instance().alarmCount();
}

/*!
 * \qmlmethod AlarmModel::refresh()
 * The function refreshes the model by invalidating the alarm cache. Use this
 * function only if the refresh is absolutely required, otherwise it will cause
 * performance problems.
 */
void UCAlarmModel::refresh()
{
    AlarmManager::instance().fetchAlarms();
}

/*!
 * \internal
 * The slot prepares the model reset.
 */
void UCAlarmModel::refreshStart()
{
    beginResetModel();
}

/*!
 * \internal
 * The slot finalizes the model reset.
 */
void UCAlarmModel::refreshEnd()
{
    endResetModel();
    Q_EMIT countChanged();
}

/*!
 * \internal
 * Slot updating individual alarm's data.
 */
void UCAlarmModel::update(int index)
{
    // create index and emit dataUpdate()
    QModelIndex modelIndex = createIndex(index, 0);
    Q_EMIT dataChanged(modelIndex, modelIndex);
}

/*!
 * \internal
 * Slot starting removing individual alarm.
 */
void UCAlarmModel::removeStarted(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
}

/*!
 * \internal
 * Slot finalizing removing individual alarm.
 */
void UCAlarmModel::removeFinished()
{
    endRemoveRows();
    Q_EMIT countChanged();
}

/*!
 * \internal
 * Slot starting inserting an individual alarm.
 */
void UCAlarmModel::insertStarted(int index)
{
    beginInsertRows(QModelIndex(), index, index);
}

/*!
 * \internal
 * Slot finalizing inserting an individual alarm.
 */
void UCAlarmModel::insertFinished()
{
    endInsertRows();
    Q_EMIT countChanged();
}

/*!
 * \internal
 * Slot starting moving an individual alarm.
 */
void UCAlarmModel::moveStarted(int from, int to)
{
    if (m_moved) {
        return;
    }
    m_moved = beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
}

/*!
 * \internal
 * Slot finalizing moving an individual alarm.
 */
void UCAlarmModel::moveFinished()
{
    if (m_moved) {
        endMoveRows();
    }
    m_moved = false;
}
