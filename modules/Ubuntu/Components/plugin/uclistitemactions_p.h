/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef UCLISTITEMACTIONS_P_H
#define UCLISTITEMACTIONS_P_H

#include "uclistitemactions.h"
#include "QtCore/private/qobject_p.h"

class UCListItemActionsPrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(UCListItemActions)
public:
    UCListItemActionsPrivate();
    ~UCListItemActionsPrivate();
    static UCListItemActionsPrivate* get(UCListItemActions *actions)
    {
        Q_ASSERT(actions);
        return actions->d_func();
    }

    QQmlComponent *delegate;
    QQuickItem *panelItem;
    QList<UCAction*> actions;
    QList<QObject*> data;

    // options list property functions
    static void funcAppend(QQmlListProperty<UCAction>*, UCAction*);
    static int funcCount(QQmlListProperty<UCAction>*);
    static QObject *funcAt(QQmlListProperty<UCAction>*, int);
    static void funcClear(QQmlListProperty<UCAction>*);
};

#endif // UCLISTITEMACTIONS_P_H
