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
#include "ubuntutoolkitglobal.h"

UT_NAMESPACE_BEGIN

class UCAction;
class UCActionContext;
class UBUNTUTOOLKIT_EXPORT UCActionManager : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(UCAction)> actions READ actions)
    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(UCActionContext)> localContexts READ localContexts)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCActionContext) *globalContext READ globalContext CONSTANT)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit UCActionManager(QObject *parent = 0);

    void classBegin() override {}
    void componentComplete() override;

    QQmlListProperty<UCAction> actions();
    QQmlListProperty<UCActionContext> localContexts();
    UCActionContext *globalContext() const;

Q_SIGNALS:
    void quit();

public Q_SLOTS:
    void addAction(UT_PREPEND_NAMESPACE(UCAction) *action);
    void removeAction(UT_PREPEND_NAMESPACE(UCAction) *action);
    void addLocalContext(UT_PREPEND_NAMESPACE(UCActionContext) *context);
    void removeLocalContext(UT_PREPEND_NAMESPACE(UCActionContext) *context);

private:
    static void contextAppend(QQmlListProperty<UCActionContext> *list, UCActionContext *context);
    static void contextClear(QQmlListProperty<UCActionContext> *list);
    static int contextCount(QQmlListProperty<UCActionContext> *list);

    static void actionAppend(QQmlListProperty<UCAction> *list, UCAction *action);
    static void actionClear(QQmlListProperty<UCAction> *list);
    static int actionCount(QQmlListProperty<UCAction> *list);
};

UT_NAMESPACE_END

#endif // UCACTIONMANAGER_H
