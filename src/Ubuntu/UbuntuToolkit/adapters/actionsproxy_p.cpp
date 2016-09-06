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
#include "ucactioncontext_p.h"

#include <QDebug>

Q_LOGGING_CATEGORY(ucActionProxy, "ubuntu.components.ActionProxy", QtMsgType::QtWarningMsg)

#define AP_TRACE(params) qCDebug(ucActionProxy) << params

UT_NAMESPACE_BEGIN

ActionProxy::ActionProxy()
    : globalContext(new UCActionContext)
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
    AP_TRACE("ADD CONTEXT" << context);
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
    AP_TRACE("REMOVE CONTEXT FROM REGISTRY" << context);
}

// publishes/removes context actions on activation/deactivation
void ActionProxy::activateContext(UCActionContext *context)
{
    if (!context) {
        return;
    }

    // if a context to be activated is a popup one, we must deactivate all other ones
    // and then activate this
    if (context->active()) {
        // publish the context's actions to the system
        instance().publishContextActions(context);
        context->markActionsPublished(true);

        if (context->isPopup()) {
            instance().addPopupContext(static_cast<UCPopupContext*>(context));
        } else {
            AP_TRACE("ACTIVATE CONTEXT" << context);
        }
    } else {
        // remove actions from the system
        instance().clearContextActions(context);
        context->markActionsPublished(false);

        if (context->isPopup()) {
            instance().removePopupContext(static_cast<UCPopupContext*>(context));
        } else {
            AP_TRACE("DEACTIVATE CONTEXT" << context);
        }
    }
}

void ActionProxy::addPopupContext(UCPopupContext *context)
{
    // deactivate last context and append
    UCPopupContext *lastActive = m_popupContexts.isEmpty() ?
                Q_NULLPTR : m_popupContexts.top();
    if (lastActive) {
        lastActive->setEffectiveActive(false);
        AP_TRACE("DEACTIVATE POPUPCONTEXT" << lastActive);
    }
    m_popupContexts.push(context);
    AP_TRACE("ACTIVATE POPUPCONTEXT" << context);
}

void ActionProxy::removePopupContext(UCPopupContext *context)
{
    UCPopupContext *last = m_popupContexts.isEmpty() ?
                Q_NULLPTR : m_popupContexts.top();

    if (last == context) {
        // we are about to remove the last one
        AP_TRACE("DEACTIVATE POPUPCONTEXT" << last);
        m_popupContexts.pop();
        // and then re-activate the second last one
        last = m_popupContexts.isEmpty() ? Q_NULLPTR : m_popupContexts.top();
        if (last) {
            AP_TRACE("REACTIVATE POPUPCONTEXT" << last);
            last->setEffectiveActive(true);
        }
    } else {
        // we simply remove the context and leave
        AP_TRACE("REMOVE POPUPCONTEXT" << context);
        m_popupContexts.removeAll(context);
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

UT_NAMESPACE_END
