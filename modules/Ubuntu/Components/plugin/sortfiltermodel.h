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

class SortBehavior : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(Qt::SortOrder order READ order WRITE setOrder NOTIFY orderChanged)

public:
    QString property() const;
    void setProperty(const QString& property);
    Qt::SortOrder order() const;
    void setOrder(Qt::SortOrder order);

Q_SIGNALS:
    void propertyChanged();
    void orderChanged();

private:
    QString m_property;
    Qt::SortOrder m_order;
};

class FilterBehavior : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(QRegExp pattern READ pattern WRITE setPattern NOTIFY patternChanged)

public:
    QString property() const;
    void setProperty(const QString& property);
    QRegExp pattern() const;
    void setPattern(QRegExp pattern);

Q_SIGNALS:
    void propertyChanged();
    void patternChanged();

private:
    QString m_property;
    QRegExp m_pattern;
};


class Q_DECL_EXPORT QSortFilterProxyModelQML : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* model READ sourceModel WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(SortBehavior* sort READ sortBehavior)
    Q_PROPERTY(FilterBehavior* filter READ filterBehavior)

public:
    explicit QSortFilterProxyModelQML(QObject *parent = 0);

    Q_INVOKABLE QVariantMap get(int row); // Use with caution, it can be slow to query all the roles
    Q_INVOKABLE QVariant data(int row, int role);
    Q_INVOKABLE int count();
    Q_INVOKABLE int findFirst(const QString& roleName, const QVariant& value) const;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    /* getters */
    QHash<int, QByteArray> roleNames() const;

    /* setters */
    void setFilterProperty(const QString& property);
    void setModel(QAbstractItemModel *model);

    Q_INVOKABLE int mapFromSource(int row);
    Q_INVOKABLE int mapToSource(int row);

Q_SIGNALS:
    void countChanged();
    void modelChanged();

private:
    SortBehavior m_sortBehavior;
    SortBehavior* sortBehavior();
    void sortChanged();
    FilterBehavior m_filterBehavior;
    FilterBehavior* filterBehavior();
    void filterChanged();
    int roleByName(const QString& roleName) const;
};

#endif // QSORTFILTERPROXYMODELQML_H
