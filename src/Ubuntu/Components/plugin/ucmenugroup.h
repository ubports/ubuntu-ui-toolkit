/*
 * Copyright 2016 Canonical Ltd.
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
 */

#ifndef UCMENUGROUP_H
#define UCMENUGROUP_H

#include <QObject>
#include <QVector>
#include <QQmlListProperty>

class UCMenuGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit UCMenuGroup(QObject *parent = 0);

    QQmlListProperty<QObject> data();

    const QVector<QObject*> &list() const;

public Q_SLOTS:
    void addObject(QObject *object);
    void removeObject(QObject *object);

Q_SIGNALS:
    void added(QObject *object);
    void removed(QObject *object);
    void changed();

protected:
    QVector<QObject*> m_data;

    static void append(QQmlListProperty<QObject> *list, QObject *action);
    static int count(QQmlListProperty<QObject> *list);
    static QObject* at(QQmlListProperty<QObject> *list, int);
    static void clear(QQmlListProperty<QObject> *list);
};

#endif // UCMENUGROUP_H
