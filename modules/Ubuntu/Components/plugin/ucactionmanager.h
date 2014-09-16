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

#ifndef UCACTIONMANAGER_H
#define UCACTIONMANAGER_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>

class UCAction;
class UCActionContext;
class UCActionManager : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> actions READ actions)
    Q_PROPERTY(QQmlListProperty<QObject> localContexts READ localContexts)
    Q_PROPERTY(UCActionContext *globalContext READ globalContext CONSTANT)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit UCActionManager(QObject *parent = 0);

    void classBegin() {}
    void componentComplete();

    QQmlListProperty<QObject> actions();
    QQmlListProperty<QObject> localContexts();
    UCActionContext *globalContext() const;

Q_SIGNALS:
    void quit();

public Q_SLOTS:
    void addAction(QObject *action);
    void removeAction(QObject *action);
    void addLocalContext(QObject *context);
    void removeLocalContext(QObject *context);

private:
    static void contextAppend(QQmlListProperty<QObject> *list, QObject *context);
    static void contextClear(QQmlListProperty<QObject> *list);
    static int contextCount(QQmlListProperty<QObject> *list);

    static void actionAppend(QQmlListProperty<QObject> *list, QObject *action);
    static void actionClear(QQmlListProperty<QObject> *list);
    static int actionCount(QQmlListProperty<QObject> *list);
};

#endif // UCACTIONMANAGER_H
