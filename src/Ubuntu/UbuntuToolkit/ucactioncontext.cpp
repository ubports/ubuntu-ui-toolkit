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
#include <QtQuick/QQuickItem>

Q_LOGGING_CATEGORY(ucActionContext, "ubuntu.components.ActionContext", QtMsgType::QtWarningMsg)

#define CONTEXT_TRACE(params) qCDebug(ucActionContext) << params

UT_NAMESPACE_BEGIN

UCActionContextAttached::UCActionContextAttached(QObject *owner)
    : QObject(owner)
    , m_owner(qobject_cast<QQuickItem*>(owner))
    , m_context(Q_NULLPTR)
{
}

/*!
 * \qmltype ActionContext
 * \instantiates UbuntuToolkit::UCActionContext
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief ActionContext groups actions together and by providing multiple contexts
 * the developer is able to control the visibility of the actions. The \l ActionManager
 * then exposes the actions from these different contexts.
 *
 * ActionContext drives the state of its \l actions. Shortcuts and mnemonics are
 * only registered if the context is active or if the action is assigned to an
 * \l ActionItem all of whose parent contexts are active. In the following
 * example the ActionContext drives the underlaying \c action1 and \c action2
 * shortcuts, and \c orphanAction will never trigger as it is neither enclosed
 * in an active context nor assigned to an ActionItem.
 * \qml
 * import QtQuick 2.4
 * import ubuntu.Componenst 1.3
 *
 * Rectangle {
 *     id: root
 *     width: units.gu(40)
 *     height: units.gu(71)
 *     ActionContext {
 *         id: rootContext
 *         active: true
 *         actions: Action {
 *             shortcut: 'Ctrl+A'
 *             text: rootContext.active ? "Deactivate" : "Activate"
 *             onTriggered: rootContext.active = !rootContext.active
 *         }
 *     }
 *
 *     Action {
 *         id: orphanAction
 *         text: "Orphan"
 *         shortcut: 'Ctrl+O'
 *         onTriggered: console.log("This will not be called")
 *     }
 *
 *     Column {
 *         Button {
 *             text: rootContext.active ? "Deactivate" : "Activate"
 *             onClicked: rootContext.active = !rootContext.active
 *         }
 *         Button {
 *             action: Action {
 *                 id: action1
 *                 text: "F&irst Button"
 *                 onTriggered: console.log("First Button triggered")
 *             }
 *         }
 *         Button {
 *             action: Action {
 *                 id: action2
 *                 text: "S&econd Button"
 *                 shortcut: 'Ctrl+Alt+2'
 *                 onTriggered: console.log("Second Button triggered")
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * The toolkit assigns an ActionContext to each Page component, which is
 * activated/deactivated together with the Page itself, driving the shortcut
 * activations on the components and actions declared in the Page.
 * \sa PopupContext
 */
UCActionContext::UCActionContext(QObject *parent)
    : QObject(parent)
    , m_active(false)
    , m_effectiveActive(true)
    , m_popup(false)
{
}
UCActionContext::~UCActionContext()
{
    ActionProxy::removeContext(this);
}

UCActionContextAttached *UCActionContext::qmlAttachedProperties(QObject *owner)
{
    return new UCActionContextAttached(owner);
}

void UCActionContext::classBegin()
{
    // add the context to the management
    ActionProxy::addContext(this);
    // make sure we attach to the parent
    UCActionContextAttached *attached = static_cast<UCActionContextAttached*>(
            qmlAttachedPropertiesObject<UCActionContext>(parent(), true));
    attached->m_context = this;
}

void UCActionContext::componentComplete()
{
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
 * that has been added to it. There can be more than one local context active at a.
 * time. When a local context is set active the manager will notice this and will
 * export the actions from the context.
 * \note An Action declared to a component falling under an item that is a child of
 * an inactive ActiveContext can be triggered manually using the mouse or connections.
 */
bool UCActionContext::active()
{
    return m_active && m_effectiveActive;
}
void UCActionContext::setActive(bool active)
{
    if (m_active == active) {
        return;
    }
    // skip deactivation for global context
    if (!active && (ActionProxy::instance().globalContext == this)) {
        return;
    }
    CONTEXT_TRACE("ACTIVATE CONTEXT" << this << active);

    m_active = active;
    ActionProxy::activateContext(this);
    Q_EMIT activeChanged();
}

// similar to setActive() but does not alter the actions from the proxy
void UCActionContext::setEffectiveActive(bool active)
{
    if (m_effectiveActive == active) {
        return;
    }
    // skip deactivation for global context
    if (!active && (ActionProxy::instance().globalContext == this)) {
        return;
    }
    CONTEXT_TRACE("EFECTIVE ACTIVATE CONTEXT" << this << active);

    m_effectiveActive = active;
    Q_EMIT activeChanged();
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


/*!
 * \qmltype PopupContext
 * \instantiates UbuntuToolkit::UCPopupContext
 * \inqmlmodule Ubuntu.Components
 * \since Ubuntu.Components 1.3
 * \inherits ActionContext
 * \ingroup ubuntu
 * \brief A special ActionContext used in Dialogs and Popups.
 *
 * A PopupContext is similar to the ActionContext, with the only difference being
 * that there can be only one PopupContext active at a time in an application.
 * A PopupContext can have several active ActionContext children declared, however
 * when deactivated all child contexts will be deactivated as well, and no Action
 * declared in these contexts will be available through shortcuts.
 *
 * The toolkit provides this kind of contexts in MainView, Popup and Dialog. It is
 * highly recommended for applications to have a PopupContext defined in their rootItem.
 */
UCPopupContext::UCPopupContext(QObject *parent)
    : UCActionContext(parent)
{
    m_popup = true;
}

UT_NAMESPACE_END
