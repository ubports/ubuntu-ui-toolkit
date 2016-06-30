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

UT_NAMESPACE_BEGIN

/*!
 * \qmltype ActionManager
 * \instantiates UbuntuToolkit::UCActionManager
 * \inqmlmodule Ubuntu.Components
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
QQmlListProperty<UCAction> UCActionManager::actions()
{
    return QQmlListProperty<UCAction>(this, 0, actionAppend, actionCount, 0, actionClear);
}

void UCActionManager::actionAppend(QQmlListProperty<UCAction> *list, UCAction *action)
{
    Q_UNUSED(list);
    ActionProxy::instance().globalContext->m_actions.insert(action);
}

void UCActionManager::actionClear(QQmlListProperty<UCAction> *list)
{
    Q_UNUSED(list);
    UCActionContext *context = ActionProxy::instance().globalContext;
    context->m_actions.clear();
}

int UCActionManager::actionCount(QQmlListProperty<UCAction> *list)
{
    Q_UNUSED(list);
    return ActionProxy::instance().globalContext->m_actions.count();
}

/*!
 * \qmlproperty list<ActionContext> ActionManager::localContexts
 * List of local contexts.
 */
QQmlListProperty<UCActionContext> UCActionManager::localContexts()
{
    return QQmlListProperty<UCActionContext>(this, 0, contextAppend, contextCount, 0, contextClear);
}

void UCActionManager::contextAppend(QQmlListProperty<UCActionContext> *list, UCActionContext *context)
{
    Q_UNUSED(list);
    ActionProxy::addContext(context);
}

void UCActionManager::contextClear(QQmlListProperty<UCActionContext> *list)
{
    Q_UNUSED(list);
    Q_FOREACH(UCActionContext *context, ActionProxy::instance().localContexts().toList()) {
        ActionProxy::removeContext(context);
    }
}

int UCActionManager::contextCount(QQmlListProperty<UCActionContext> *list)
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
void UCActionManager::addAction(UCAction *action)
{
    if (!action) {
        return;
    }
    ActionProxy::instance().globalContext->addAction(action);
}

/*!
 * \qmlmethod ActionManager::removeAction(Action action)
 * \deprecated
 * This is a shorthand for \c ActionManager.globalContext.removeAction(action) call.
 */
void UCActionManager::removeAction(UCAction *action)
{
    if (!action) {
        return;
    }
    ActionProxy::instance().globalContext->removeAction(action);
}

/*!
 * \qmlmethod ActionManager::addLocalContext(ActionContext context)
 * \deprecated
 * Adds the local context.
 *
 * This is deprecated. ActionContext instances are added autimatically to the
 * action management stystem when declared and removed when destroyed.
 */
void UCActionManager::addLocalContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    ActionProxy::addContext(context);
}

/*!
 * \qmlmethod ActionManager::removeLocalContext(ActionContext context)
 * \deprecated
 * Removes the local context.
 *
 * This is deprecated. ActionContext instances are added autimatically to the
 * action management stystem when declared and removed when destroyed.
 */
void UCActionManager::removeLocalContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    ActionProxy::removeContext(context);
}

UT_NAMESPACE_END
