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

#ifndef TABSMODEL_H
#define TABSMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class TabsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged);

public:
    TabsModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex&, int) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void append(const QString &title);
    Q_INVOKABLE QVariantMap get(int index);

Q_SIGNALS:
    void countChanged();

private:
    QStringList m_list;
};

#endif
