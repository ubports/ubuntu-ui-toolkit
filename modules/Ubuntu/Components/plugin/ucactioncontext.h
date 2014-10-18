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

#ifndef UCACTIONCONTEXT_H
#define UCACTIONCONTEXT_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>
#include <QtCore/QSet>
#include <QtQml>

class UCAction;
class UCActionContext : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<QObject> actions READ actions)
    Q_PROPERTY(bool active MEMBER m_active WRITE setActive NOTIFY activeChanged)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit UCActionContext(QObject *parent = 0);
    ~UCActionContext();

    void classBegin(){}
    void componentComplete();
    void markActionsPublished(bool mark);

    QQmlListProperty<QObject> actions();

    void setActive(bool active);

Q_SIGNALS:
    void activeChanged(bool);

public Q_SLOTS:
    void addAction(QObject *action);
    void removeAction(QObject *action);

private:
    bool m_active;
    QSet<UCAction*> m_actions;
    friend class UCActionManager;

    static void append(QQmlListProperty<QObject> *list, QObject *action);
    static void clear(QQmlListProperty<QObject> *list);
    static int count(QQmlListProperty<QObject> *list);
};

QML_DECLARE_TYPE(UCActionContext)

#endif // UCACTIONCONTEXT_H
