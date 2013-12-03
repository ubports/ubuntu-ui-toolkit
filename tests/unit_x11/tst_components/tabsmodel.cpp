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
 */

#include "tabsmodel.h"

TabsModel::TabsModel()
{
}

int TabsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_list.count();
}

QVariant TabsModel::data(const QModelIndex &index, int role) const 
{
    if (role == Qt::DisplayRole) {
        return m_list[index.row()];
    }
    return QVariant();
}

void TabsModel::append(const QString &title)
{
    const int count = m_list.count();
    beginInsertRows(QModelIndex(), count, count);
    m_list << title;
    endInsertRows();
    Q_EMIT countChanged();
}

QVariantMap TabsModel::get(int index)
{
    QVariantMap res;
    res.insert("title", m_list[index]);
    return res;
}

QHash<int, QByteArray> TabsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "title";
    return roles;
}

