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

#include "ucactioncontext.h"
#include "ucaction.h"
#include "adapters/actionsproxy_p.h"

/*!
 * \qmltype ActionContext
 * \instantiates UCActionContext
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief ActionContext groups actions together and by providing multiple contexts
 * the developer is able to control the visibility of the actions. The \l ActionManager
 * then exposes the actions from these different contexts.
 */
UCActionContext::UCActionContext(QObject *parent)
    : QObject(parent)
    , m_active(false)
{
}
UCActionContext::~UCActionContext()
{
    ActionProxy::removeContext(this);
}

void UCActionContext::componentComplete()
{
    // add the context to the management
    ActionProxy::addContext(this);
}

/*
 * The function marks all context actions being (un)published.
 */
void UCActionContext::markActionsPublished(bool mark)
{
    Q_FOREACH(UCAction *action, m_actions) {
        action->m_published = mark;
    }
}

/*!
 * \qmlproperty list<Action> ActionContext::actions
 * \default
 * List of Actions in this ActionContext.
 */
QQmlListProperty<UCAction> UCActionContext::actions()
{
    return QQmlListProperty<UCAction>(this, 0, UCActionContext::append, UCActionContext::count, 0, UCActionContext::clear);
}

void UCActionContext::append(QQmlListProperty<UCAction> *list, UCAction *action)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        context->m_actions.insert(action);
    }
}

void UCActionContext::clear(QQmlListProperty<UCAction> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        context->m_actions.clear();
    }
}

int UCActionContext::count(QQmlListProperty<UCAction> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        return context->m_actions.count();
    }
    return 0;
}

/*!
 * \qmlproperty bool ActionContext::active
 * If true the context is active. If false the context is inactive. Defaults to
 * false.
 *
 * When context has been added to the \l ActionManager setting this value controls
 * whether or not the actions in a context are available to external components.
 *
 * The \l ActionManager monitors the active property of each of the local contexts
 * that has been added to it. There can be only one active local context at a time.
 * When one of the local contexts sets itself active the manager will notice this,
 * export the actions from that given context and set the previously active local
 * context as inactive. This way setting active to true on a local context is
 * sufficient to manage the active local context of the manager and no additional
 * calls are necessary to manually inactivate the other contexts.
 */
void UCActionContext::setActive(bool active)
{
    if (m_active == active) {
        return;
    }
    // skip deactivation for global context
    if (!active && (ActionProxy::instance().globalContext == this)) {
        return;
    }
    m_active = active;
    Q_EMIT activeChanged(active);
}

/*!
 * \qmlmethod ActionContext::addAction(Action action)
 * \deprecated
 * Adds an Action to the context programatically.
 */
void UCActionContext::addAction(UCAction *action)
{
    if (m_actions.contains(action)) {
        return;
    }
    m_actions.insert(action);
}

/*!
 * \qmlmethod ActionContext::removeAction(Action action)
 * \deprecated
 * Removes an action from the context programatically.
 */
void UCActionContext::removeAction(UCAction *action)
{
    if (!action) {
        return;
    }
    m_actions.remove(action);
}
