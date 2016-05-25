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

#ifndef UCACTIONLIST_H
#define UCACTIONLIST_H

#include <QObject>
#include <QtQml/QQmlListProperty>

class UCAction;

class UCActionList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit UCActionList(QObject *parent = 0);

    QQmlListProperty<UCAction> actions();

public Q_SLOTS:
    void addAction(UCAction *action);
    void removeAction(UCAction *action);

protected:
    QList<UCAction*> m_actions;

    static void append(QQmlListProperty<UCAction> *list, UCAction *action);
    static void clear(QQmlListProperty<UCAction> *list);
    static int count(QQmlListProperty<UCAction> *list);
};

#endif // UCACTIONLIST_H
