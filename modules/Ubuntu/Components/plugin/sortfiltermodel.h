/*
 * Copyright (C) 2012-2014 Canonical, Ltd.
 *
 * Authors:
 *   Michal Hruby <michal.hruby@canonical.com>
 *   Christian Dywan <christian.dywan@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QSORTFILTERPROXYMODELQML_H
#define QSORTFILTERPROXYMODELQML_H

#include <QSortFilterProxyModel>

class Q_DECL_EXPORT QSortFilterProxyModelQML : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* model READ sourceModel WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);

    Q_INVOKABLE QVariantMap get(int row); // Use with caution, it can be slow to query all the roles
    Q_INVOKABLE QVariant data(int row, int role);
    Q_INVOKABLE int count();
    Q_INVOKABLE int findFirst(int role, const QVariant& value) const;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    /* getters */
    QHash<int, QByteArray> roleNames() const;

    /* setters */
    void setSortOrder(Qt::SortOrder newOrder);
    void setModel(QAbstractItemModel *model);

    Q_INVOKABLE int mapFromSource(int row);
    Q_INVOKABLE int mapToSource(int row);

Q_SIGNALS:
    void countChanged();
    void modelChanged();
};

#endif // QSORTFILTERPROXYMODELQML_H
