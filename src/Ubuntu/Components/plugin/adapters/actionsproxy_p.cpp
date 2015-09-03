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
    // if there is still an active context clear it
    if (!m_activeContext.isNull()) {
        m_activeContext->setActive(false);
    }
    // clear context explicitly, as global context is not connected to
    clearContextActions(globalContext);
    delete globalContext;
}

UCActionContext *ActionProxy::currentContext()
{
    return instance().m_activeContext;
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
    // watch context activation changes
    instance().watchContextActivation(context, true);
}
// Remove a local context. If the context was active, removes the actions from the system.
void ActionProxy::removeContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    // make sure the context is deactivated
    context->setActive(false);
    instance().watchContextActivation(context, false);
    instance().m_localContexts.remove(context);
}

// toggles context activation watching for a given context
void ActionProxy::watchContextActivation(UCActionContext *context, bool watch)
{
    if (!context) {
        return;
    }
    if (watch) {
        // connect to action proxy
        QObject::connect(context, SIGNAL(activeChanged()),
                         this, SLOT(handleContextActivation()),
                         Qt::DirectConnection);
    } else {
        // disconnect
        QObject::disconnect(context, SIGNAL(activeChanged()),
                         this, SLOT(handleContextActivation()));
    }
}

// handles the local context activation
void ActionProxy::handleContextActivation()
{
    // sender is the context changing activation
    UCActionContext *context = qobject_cast<UCActionContext*>(sender());
    if (!context) {
        return;
    }
    // deactivate the previous context if any
    if (!m_activeContext.isNull()) {
        if (!context->active()) {
            // the slot has been called due to the previous active deactivation,
            // so perform system cleanup
            clearContextActions(m_activeContext);
            m_activeContext->markActionsPublished(false);
            // finally clear the context and leave
            m_activeContext.clear();
            return;
        } else {
            // deactivate previous actiev context, this will cause the slot to
            // be called with active = false within this call context
            m_activeContext->setActive(false);
        }
    }
    if (context->active()) {
        // publish the context's actions to the system
        publishContextActions(context);
        context->markActionsPublished(true);
        // and finally set it as active
        m_activeContext = context;
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
