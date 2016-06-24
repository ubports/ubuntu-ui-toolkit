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

UT_NAMESPACE_BEGIN

/*!
 * \qmltype ActionList
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief List of \l Action items
 * An ActionList provies a way of grouping actions together.
 * \qml
 * ActionList {
 *     Action {
 *         id: action1
 *     }
 *     Action {
 *         id: action2
 *     }
 * }
 * \endqml
 */
/*!
 * \qmlsignal ActionList::added(Action action)
 * \since Ubuntu.Components 1.3
 * Signal called when an action is added to the list
 */
/*!
 * \qmlsignal ActionList::removed(Action action)
 * \since Ubuntu.Components 1.3
 * Signal called when an action is removed from the list
 */
UCActionList::UCActionList(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlmethod ActionList::addAction(Action action)
 * \since Ubuntu.Components 1.3
 * Adds an Action to the list programatically.
 * \qml
 * Item {
 *     Instantiator {
 *         model: 4
 *         onObjectAdded: actionList.addAction(object)
 *         onObjectRemoved: actionList.removeAction(object)
 *
 *         Action {}
 *     }
 *
 *     ActionList {
 *         id: actionList
 *     }
 * }
 * \endqml
 * \sa ActionList::removeAction
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
 * \since Ubuntu.Components 1.3
 * Removes an action from the list programatically.
 * \sa ActionList::addAction
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

UT_NAMESPACE_END
