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

#ifndef ACTIONLIST_H
#define ACTIONLIST_H

#include <QObject>
#include <QtQml/QQmlListProperty>
#include "ubuntutoolkitglobal.h"

UT_NAMESPACE_BEGIN

class UCAction;
class UBUNTUTOOLKIT_EXPORT ActionList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(UCAction)> actions READ actions)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit ActionList(QObject *parent = 0);

    QQmlListProperty<UCAction> actions();

    const QList<UCAction*> &list() const;

public Q_SLOTS:
    void addAction(UT_PREPEND_NAMESPACE(UCAction) *action);
    void removeAction(UT_PREPEND_NAMESPACE(UCAction) *action);

Q_SIGNALS:
    void added(UCAction *action);
    void removed(UCAction *action);

protected:
    QList<UCAction*> m_actions;

    static void append(QQmlListProperty<UCAction> *list, UCAction *action);
    static void clear(QQmlListProperty<UCAction> *list);
    static UCAction* at(QQmlListProperty<UCAction> *list, int index);
    static int count(QQmlListProperty<UCAction> *list);
};

UT_NAMESPACE_END

#endif // ACTIONLIST_H
