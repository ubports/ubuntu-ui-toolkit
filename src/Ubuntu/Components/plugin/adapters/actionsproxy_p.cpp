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

#include "actionsproxy_p.h"
#include "ucactioncontext.h"

#include <QDebug>

ActionProxy::ActionProxy()
    : QObject(0)
    , globalContext(new UCActionContext)
{
    // for testing purposes
    globalContext->setObjectName("GlobalActionContext");
}
ActionProxy::~ActionProxy()
{
    // clear context explicitly, as global context is not connected to
    clearContextActions(globalContext);
    delete globalContext;
}

const QSet<UCActionContext*> &ActionProxy::localContexts()
{
    return instance().m_localContexts;
}

// function called by the ActionManager when completed to publish global ActionContext
// actions.
void ActionProxy::publishGlobalContext()
{
    if (instance().globalContext) {
        instance().publishContextActions(instance().globalContext);
        instance().globalContext->markActionsPublished(true);
    }
}

// add a local context
void ActionProxy::addContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    if (instance().m_localContexts.contains(context)) {
        return;
    }
    instance().m_localContexts.insert(context);
}
// Remove a local context. If the context was active, removes the actions from the system.
void ActionProxy::removeContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    // make sure the context is deactivated
    context->setActive(false);
    instance().m_localContexts.remove(context);
}

// publishes/removes context actions on activation/deactivation
void ActionProxy::activateContext(UCActionContext *context, bool activate)
{
    if (!context) {
        return;
    }
    if (!instance().m_activeContexts.contains(context) && activate) {
        // publish the context's actions to the system
        instance().publishContextActions(context);
        context->markActionsPublished(true);
        // register context as active
        instance().m_activeContexts << context;
    } else if (!activate && instance().m_activeContexts.contains(context)){
        // remove actions from the system
        instance().clearContextActions(context);
        context->markActionsPublished(false);
        instance().m_activeContexts.remove(context);
    }
}

// empty functions for context activation/deactivation, connect to HUD
void ActionProxy::clearContextActions(UCActionContext *context)
{
    Q_UNUSED(context);
}
/*
 * Publish actions of a context to the system. Implementations should make sure
 * only unpublished actions are exported.
 */
void ActionProxy::publishContextActions(UCActionContext *context)
{
    Q_UNUSED(context);
}
