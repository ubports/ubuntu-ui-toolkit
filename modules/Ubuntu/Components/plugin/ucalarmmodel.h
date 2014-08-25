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

#ifndef UCALARMSMODEL_H
#define UCALARMSMODEL_H

#include <QtCore/QAbstractListModel>
#include "ucalarm.h"

class UCAlarm;
class UCAlarmModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit UCAlarmModel(QObject *parent = 0);
    ~UCAlarmModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    // invokables
    Q_INVOKABLE UCAlarm *get(int index);

    // property getters
    int count() const;
    
Q_SIGNALS:
    void countChanged();
    
private Q_SLOTS:
    void refresh();
    void update(const QList<QVariant> cookies);

private:
    bool m_ready;
    QHash<int, QByteArray> m_roles;
    QList<UCAlarm*> m_alarms;
    void clear();
};

#endif // UCALARMSMODEL_H
