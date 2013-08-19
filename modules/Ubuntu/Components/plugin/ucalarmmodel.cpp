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

/*!
 * \qmltype AlarmModel
 * \instantiates UCAlarmModel
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu-services
 * \brief AlarmModel holds the list of alarms defined.
 *
 * The AlarmModel is a simple container of \l Alarm definitions stored in the alarm
 * collection. The data provided by the model are read only, adding, modifying or
 * removing data is only possible through \l Alarm functions.
 *
 * The number of alarm events can be obtained from the \l count property. To get
 * a specific alarm event data from the model, use the \l get function.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 * import Ubuntu.Components.ListItems 0.1
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
 */

UCAlarmModel::UCAlarmModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // keep in sync with alarms collection changes
    connect(&AlarmManager::instance(), SIGNAL(alarmsChanged()), this, SLOT(refresh()));
}

int UCAlarmModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return AlarmManager::instance().alarms().count();
}
QVariant UCAlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int idx = index.row();
    const QList<AlarmData> alarms = AlarmManager::instance().alarms();
    if ((idx >= alarms.count()) || (idx < 0)) {
        return QVariant();
    }
    return alarms[idx].roleData(role);
}

QHash<int, QByteArray> UCAlarmModel::roleNames() const
{
    return AlarmData::roles();
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
    QList<AlarmData> alarms = AlarmManager::instance().alarms();
    if ((index >= 0) && (index < alarms.count())) {
        UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(&m_alarm);
        pAlarm->rawData = alarms[index];
        return &m_alarm;
    }
    return 0;
}

/*!
 * \qmlproperty int AlarmModel::count
 * The number of data entries in the model.
 */
int UCAlarmModel::count() const
{
    return AlarmManager::instance().alarms().count();
}

/*!
 * \internal
 * The slot prepares the views for the dataChanged() signal.
 */
void UCAlarmModel::refresh()
{
    beginResetModel();
    endResetModel();
}
