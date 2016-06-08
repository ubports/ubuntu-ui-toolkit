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

#include "ucactionlist.h"
#include "ucaction.h"

/*!
 * \qmltype ActionList
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief List of \l Action items
 */
/*!
 * \qmlsignal ActionList::added(Action action)
 * Signal called when an action is added to the list
 */
/*!
 * \qmlsignal ActionList::removed(Action action)
 * Signal called when an action is removed from the list
 */
UCActionList::UCActionList(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlmethod ActionList::addAction(Action action)
 * \deprecated
 * Adds an Action to the list programatically.
 */
void UCActionList::addAction(UCAction *action)
{
    if (m_actions.contains(action)) {
        return;
    }
    m_actions.append(action);
    Q_EMIT added(action);
}

/*!
 * \qmlmethod ActionList::removeAction(Action action)
 * \deprecated
 * Removes an action from the list programatically.
 */
void UCActionList::removeAction(UCAction *action)
{
    if (!action) {
        return;
    }
    if (m_actions.removeOne(action)) {
        Q_EMIT removed(action);
    }
}

/*!

 * \qmlproperty list<Action> ActionList::actions
 * \default
 * List of Actions in this ActionList
 * Note that when you set this property, the children of the ActionList will be ignored,
 * so do not set the list and define children.
 *
 * The advantage of setting actions over using the children is that the same
 * \l Action items can be used in several sets of actions.
 */
QQmlListProperty<UCAction> UCActionList::actions()
{
    return QQmlListProperty<UCAction>(this, 0, UCActionList::append, UCActionList::count, 0, UCActionList::clear);
}

const QList<UCAction*> &UCActionList::list() const
{
    return m_actions;
}

void UCActionList::append(QQmlListProperty<UCAction> *list, UCAction *action)
{
    UCActionList *actionList = qobject_cast<UCActionList*>(list->object);
    if (actionList) {
        actionList->addAction(action);
    }
}

void UCActionList::clear(QQmlListProperty<UCAction> *list)
{
    UCActionList *actionList = qobject_cast<UCActionList*>(list->object);
    if (actionList) {
        actionList->m_actions.clear();
    }
}

int UCActionList::count(QQmlListProperty<UCAction> *list)
{
    UCActionList *actionList = qobject_cast<UCActionList*>(list->object);
    if (actionList) {
        return actionList->m_actions.count();
    }
    return 0;
}
