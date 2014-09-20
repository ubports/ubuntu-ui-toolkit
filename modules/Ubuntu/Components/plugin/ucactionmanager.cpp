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

#include "ucactionmanager.h"
#include "adapters/actionsproxy_p.h"
#include "ucactioncontext.h"
#include "ucaction.h"

/*!
 * \qmltype ActionManager
 * \instantiates UCActionManager
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief ActionManager manages actions and action contexts withion an application.
 *
 * Acts as an interface between the application and external components. Maintains
 * the global context action registration.
 *
 * There can be many ActionManager instances in an application, and all instances
 * will have the ActionContexts shared between each other. If individual ActionManager
 * instances add more Action objects, those will be published as well.
 */
UCActionManager::UCActionManager(QObject *parent)
    : QObject(parent)
{
}

void UCActionManager::componentComplete()
{
    // publish global context to system
    ActionProxy::instance().globalContext->setActive(true);
    ActionProxy::publishGlobalContext();
}

/*!
 * \qmlproperty list<Action> ActionManager::actions
 * \default
 * A list of actions in the global context.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
QQmlListProperty<QObject> UCActionManager::actions()
{
    return QQmlListProperty<QObject>(this, 0, actionAppend, actionCount, 0, actionClear);
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::actionAppend(QQmlListProperty<QObject> *list, QObject *action)
{
    Q_UNUSED(list);
    UCAction *toolkitAction = qobject_cast<UCAction*>(action);
    if (!toolkitAction) {
        qmlInfo(action) << "Unity.Action deprecated. Please use Action from Ubuntu.Components.";
        return;
    }
    ActionProxy::instance().globalContext->m_actions.insert(toolkitAction);
}

// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::actionClear(QQmlListProperty<QObject> *list)
{
    Q_UNUSED(list);
    UCActionContext *context = ActionProxy::instance().globalContext;
    context->m_actions.clear();
}

// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
int UCActionManager::actionCount(QQmlListProperty<QObject> *list)
{
    Q_UNUSED(list);
    return ActionProxy::instance().globalContext->m_actions.count();
}

/*!
 * \qmlproperty list<ActionContext> ActionManager::localContexts
 * List of local contexts.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
QQmlListProperty<QObject> UCActionManager::localContexts()
{
    return QQmlListProperty<QObject>(this, 0, contextAppend, contextCount, 0, contextClear);
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::contextAppend(QQmlListProperty<QObject> *list, QObject *context)
{
    Q_UNUSED(list);
    UCActionContext *toolkitContext = qobject_cast<UCActionContext*>(context);
    if (!toolkitContext) {
        qmlInfo(context) << "Unity.Action deprecatedContext. Please use ActionContext from Ubuntu.Components.";
        return;
    }
    ActionProxy::addContext(toolkitContext);
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::contextClear(QQmlListProperty<QObject> *list)
{
    Q_UNUSED(list);
    Q_FOREACH(UCActionContext *context, ActionProxy::instance().localContexts().toList()) {
        ActionProxy::removeContext(context);
    }
}
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
int UCActionManager::contextCount(QQmlListProperty<QObject> *list)
{
    Q_UNUSED(list);
    return ActionProxy::instance().localContexts().count();
}

/*!
 * \qmlproperty ActionContext ActionManager::globalContext
 * The globalContext of the Application.
 * \note Setting the \l ActionContext::active on the global context has no effect.
 */
UCActionContext *UCActionManager::globalContext() const
{
    return ActionProxy::instance().globalContext;
}

/*!
 * \qmlmethod ActionManager::addAction(Action action)
 * \deprecated
 * This is a shorthand for \c ActionManager.globalContext.addAction(action) call.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::addAction(QObject *action)
{
    if (!action) {
        return;
    }
    if (!qobject_cast<UCAction*>(action)) {
        qmlInfo(action) << "Unity.Action deprecated. Please use Action from Ubuntu.Components.";
        return;
    }
    ActionProxy::instance().globalContext->addAction(action);
}

/*!
 * \qmlmethod ActionManager::removeAction(Action action)
 * \deprecated
 * This is a shorthand for \c ActionManager.globalContext.removeAction(action) call.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::removeAction(QObject *action)
{
    if (!action) {
        return;
    }
    UCAction *toolkitAction = qobject_cast<UCAction*>(action);
    if (!toolkitAction) {
        qmlInfo(action) << "Unity.Action deprecated. Please use Action from Ubuntu.Components.";
        return;
    }
    ActionProxy::instance().globalContext->removeAction(toolkitAction);
}

/*!
 * \qmlmethod ActionManager::addLocalContext(ActionContext context)
 * \deprecated
 * Adds the local context.
 *
 * This is deprecated. ActionContext instances are added autimatically to the
 * action management stystem when declared and removed when destroyed.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::addLocalContext(QObject *context)
{
    if (!context) {
        return;
    }
    UCActionContext *toolkitContext = qobject_cast<UCActionContext*>(context);
    if (!toolkitContext) {
        qmlInfo(context) << "Unity.ActionContext deprecated. Please use ActionContext from Ubuntu.Components.";
        return;
    }
    ActionProxy::addContext(toolkitContext);
}

/*!
 * \qmlmethod ActionManager::removeLocalContext(ActionContext context)
 * \deprecated
 * Removes the local context.
 *
 * This is deprecated. ActionContext instances are added autimatically to the
 * action management stystem when declared and removed when destroyed.
 */
// FIXME: do cleanup https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1369874
void UCActionManager::removeLocalContext(QObject *context)
{
    if (!context) {
        return;
    }
    UCActionContext *toolkitContext = qobject_cast<UCActionContext*>(context);
    if (!toolkitContext) {
        qmlInfo(context) << "Unity.ActionContext deprecated. Please use ActionContext from Ubuntu.Components.";
        return;
    }
    ActionProxy::removeContext(toolkitContext);
}
