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
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
QQmlListProperty<QObject> UCActionContext::actions()
{
    return QQmlListProperty<QObject>(this, 0, UCActionContext::append, UCActionContext::count, 0, UCActionContext::clear);
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionContext::append(QQmlListProperty<QObject> *list, QObject *action)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        UCAction *toolkitAction = qobject_cast<UCAction*>(action);
        if (toolkitAction) {
            context->m_actions.insert(toolkitAction);
        } else {
            qmlInfo(action) << "Invalid Action. Please use Action from Ubuntu.Components.";
        }
    }
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionContext::clear(QQmlListProperty<QObject> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        context->m_actions.clear();
    }
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
int UCActionContext::count(QQmlListProperty<QObject> *list)
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
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionContext::addAction(QObject *action)
{
    UCAction *toolkitAction = qobject_cast<UCAction*>(action);
    if (!toolkitAction) {
        qmlInfo(action) << "Unity.Action deprecated. Please use Action from Ubuntu.Components.";
        return;
    }
    if (m_actions.contains(toolkitAction)) {
        return;
    }
    m_actions.insert(toolkitAction);
}

/*!
 * \qmlmethod ActionContext::removeAction(Action action)
 * \deprecated
 * Removes an action from the context programatically.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionContext::removeAction(QObject *action)
{
    if (!action) {
        return;
    }
    UCAction *toolkitAction = qobject_cast<UCAction*>(action);
    if (!toolkitAction) {
        qmlInfo(action) << "Unity.Action deprecated. Please use Action from Ubuntu.Components.";
        return;
    }
    m_actions.remove(toolkitAction);
}
