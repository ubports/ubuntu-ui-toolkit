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
 * There can be many ActionManager instances in an application, all will have the
 * contexts shared in between each other.
 */
UCActionManager::UCActionManager(QObject *parent)
    : QObject(parent)
{
}

void UCActionManager::componentComplete()
{
    // publish global context to system
    ActionManagement::instance().globalContext->setActive(true);
    ActionManagement::publishGlobalContext();
}

/*!
 * \qmlproperty list<Action> ActionManager::actions
 * \default
 * A list of actions in teh global context.
 */
QQmlListProperty<UCAction> UCActionManager::actions()
{
    return QQmlListProperty<UCAction>(this, 0, actionAppend, actionCount, 0, actionClear);
}
void UCActionManager::actionAppend(QQmlListProperty<UCAction> *list, UCAction *action)
{
    Q_UNUSED(list);
    ActionManagement::instance().globalContext->m_actions.insert(action);
}

void UCActionManager::actionClear(QQmlListProperty<UCAction> *list)
{
    Q_UNUSED(list);
    UCActionContext *context = ActionManagement::instance().globalContext;
    context->m_actions.clear();
}

int UCActionManager::actionCount(QQmlListProperty<UCAction> *list)
{
    Q_UNUSED(list);
    return ActionManagement::instance().globalContext->m_actions.count();
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
    ActionManagement::addContext(context);
}
void UCActionManager::contextClear(QQmlListProperty<UCActionContext> *list)
{
    Q_UNUSED(list);
    Q_FOREACH(UCActionContext *context, ActionManagement::instance().localContexts().toList()) {
        ActionManagement::removeContext(context);
    }
}
int UCActionManager::contextCount(QQmlListProperty<UCActionContext> *list)
{
    Q_UNUSED(list);
    return ActionManagement::instance().localContexts().count();
}

/*!
 * \qmlproperty ActionContext ActionManager::globalContext
 * The globalContext of the Application.
 * \note Setting the \l ActionContext::active on the global context has no effect.
 */
UCActionContext *UCActionManager::globalContext() const
{
    return ActionManagement::instance().globalContext;
}

/*!
 * \qmlmethod ActionManager::addAction(Action action)
 * \deprecated
 * This is a shorthand for \code ActionManager.globalContext.addAction(action) call.
 */
void UCActionManager::addAction(UCAction *action)
{
    qmlInfo(this) << "WARNING: addAction() is deprecated. Declare actions designated for global context inside ActionManager.";
    ActionManagement::instance().globalContext->addAction(action);
}

/*!
 * \qmlmethod ActionManager::removeAction(Action action)
 * \deprecated
 * This is a shorthand for \code ActionManager.globalContext.removeAction(action) call.
 */
void UCActionManager::removeAction(UCAction *action)
{
    qmlInfo(this) << "WARNING: removeAction() is deprecated. Do not remove actions programatically.";
    ActionManagement::instance().globalContext->removeAction(action);
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
    qmlInfo(this) << "WARNING: addLocalContext() is deprecated. ActionContext declaration registers the context to the management.";
    ActionManagement::addContext(context);
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
    ActionManagement::removeContext(context);
}

