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

#ifndef ULITEMLAYOUT_H
#define ULITEMLAYOUT_H

#include <QtQuick/QQuickItem>

class ULItemLayout : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ULItemLayout)

    Q_PROPERTY(QString item READ item WRITE setItem NOTIFY itemChanged)

public:
    explicit ULItemLayout(QQuickItem *parent = 0);
    ~ULItemLayout();

    QString item() const;
    void setItem(const QString &value);

Q_SIGNALS:
    void itemChanged();

private:
    QString m_itemName;
};

#endif // ULITEMLAYOUT_H
