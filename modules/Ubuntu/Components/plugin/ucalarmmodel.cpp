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

/*!
 * \qmltype AlarmModel
 * \instantiates UCAlarmModel
 * \inqmlmodule Ubuntu.Components 1.0
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
 * import QtQuick 2.0
 * import Ubuntu.Components 1.0
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
    , m_ready(false)
{
    m_roles = AlarmData::roles();
    m_roles.insert(m_roles.count(), "model");
    // keep in sync with alarms collection changes
    // make sure the connection is asynchronous, as changes made in in-place in
    // the delegates may cause the model data to be invalid (released) as some
    // backends may do the refresh/element removals synchronously
    connect(&AlarmManager::instance(), SIGNAL(alarmsChanged()), this, SLOT(refresh()), Qt::QueuedConnection);
    // fetch alarms
    refresh();
    m_ready = true;
}
UCAlarmModel::~UCAlarmModel()
{
    clear();
}

void UCAlarmModel::clear()
{
    if (m_alarms.count()) {
        Q_FOREACH(UCAlarm *alarm, m_alarms) {
            delete alarm;
        }
        m_alarms.clear();
    }
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
    if ((idx >= m_alarms.count()) || (idx < 0)) {
        return QVariant();
    }

    const QString roleName = m_roles.value(role);
    if (roleName == "model") {
        const UCAlarm *alarm = m_alarms[idx];
        return QVariant::fromValue(const_cast<UCAlarm*>(alarm));
    } else if (!roleName.isEmpty()){
        return m_alarms[idx]->property(roleName.toLocal8Bit());
    }

    return QVariant();
}

QHash<int, QByteArray> UCAlarmModel::roleNames() const
{
    return m_roles;
}

/*!
 * \qmlmethod Alarm AlarmModel::get(int index)
 * Returns the copy of the alarm event at \a index in the model. This allows the
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
 * \b Warning: The returned object is not guarantied to remain valid, it should
 * not be used in property bindings.
 *
 * \sa Alarm
 */
UCAlarm* UCAlarmModel::get(int index)
{
    if ((index >= 0) && (index < m_alarms.count())) {
        return m_alarms[index];
    }
    return 0;
}

/*!
 * \qmlproperty int AlarmModel::count
 * The number of data entries in the model.
 */
int UCAlarmModel::count() const
{
    return m_alarms.count();
}

/*!
 * \internal
 * The slot prepares the views for the dataChanged() signal.
 */
void UCAlarmModel::refresh()
{
    if (m_ready) {
        beginResetModel();
    }

    clear();
    QList<AlarmData> alarms = AlarmManager::instance().alarms();
    Q_FOREACH(const AlarmData &data, alarms) {
        UCAlarm *alarm = new UCAlarm;
        UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(alarm);
        pAlarm->rawData = data;
        m_alarms << alarm;
    }
    Q_EMIT countChanged();

    if (m_ready) {
        endResetModel();
    }
}
