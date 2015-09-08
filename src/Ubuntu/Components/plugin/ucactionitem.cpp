/*
 * Copyright 2015 Canonical Ltd.
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

#include "ucactionitem.h"
#include "ucaction.h"

/*!
 * \qmltype ActionItem
 * \instantiates UCActionItem
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief A visual representation of an Action. The API of ActionItem is a
 * copy of the API of \l Action, with additional properties to define
 * visual aspects of the ActionItem.
 *
 * If \l action is set, the values of the other properties will by default
 * be identical to the \l Action's property values. Setting the other properties
 * will override the properties copied from the \l Action.
 */

/*!
 * \qmlsignal ActionItem::triggered(var value)
 * Called when the actionItem is triggered.
 */
UCActionItem::UCActionItem(QQuickItem *parent)
    : UCStyledItemBase(parent)
    , m_action(Q_NULLPTR)
    , m_flags(0)
{
    connect(this, &UCActionItem::visibleChanged, this, &UCActionItem::_q_visibleChanged);
    connect(this, &UCActionItem::enabledChanged, this, &UCActionItem::_q_enabledChanged);
}

void UCActionItem::componentComplete()
{
    UCStyledItemBase::componentComplete();
    // make sure we connect to the right signals, so we detach and re-attach actions
    // to make sure the SLOT macro picks up the custom trigger() slot
    if (m_action) {
        attachAction(false);
        attachAction(true);
    }
}

void UCActionItem::_q_visibleChanged()
{
    m_flags |= CustomVisible;
    disconnect(this, &UCActionItem::visibleChanged, this, &UCActionItem::_q_visibleChanged);
}

void UCActionItem::_q_enabledChanged()
{
    m_flags |= CustomEnabled;
    disconnect(this, &UCActionItem::enabledChanged, this, &UCActionItem::_q_enabledChanged);
}

// update visible property
void UCActionItem::_q_updateVisible()
{
    bool visible = m_action ? m_action->m_visible : true;
    setVisible(visible);
    // reset flag and reconnect signal handler disconnected by the
    m_flags &= ~CustomVisible;
    if (m_action) {
        connect(this, &UCActionItem::visibleChanged, this, &UCActionItem::_q_visibleChanged);
    }
}

// update enabled property
void UCActionItem::_q_updateEnabled()
{
    bool enabled = m_action ? m_action->m_enabled : true;
    setEnabled(enabled);
    // reset flag and reconnect signal handler disconnected by the
    m_flags &= ~CustomEnabled;
    if (m_action) {
        connect(this, &UCActionItem::enabledChanged, this, &UCActionItem::_q_enabledChanged);
    }
}

void UCActionItem::updateProperties()
{
    if (!(m_flags & CustomText)) {
        Q_EMIT textChanged();
    }
    if (!(m_flags & CustomIconSource)) {
        Q_EMIT iconSourceChanged();
    }
    if (!(m_flags & CustomIconName)) {
        Q_EMIT iconNameChanged();
    }
}

void UCActionItem::attachAction(bool attach)
{
    if (attach) {
        connect(this, SIGNAL(triggered(QVariant)),
                m_action, SLOT(trigger(QVariant)), Qt::DirectConnection);
        connect(m_action, &UCAction::visibleChanged,
                this, &UCActionItem::_q_updateVisible, Qt::DirectConnection);
        connect(m_action, &UCAction::enabledChanged,
                this, &UCActionItem::_q_updateEnabled, Qt::DirectConnection);
        if (!(m_flags & CustomText)) {
            connect(m_action, &UCAction::textChanged,
                    this, &UCActionItem::textChanged, Qt::DirectConnection);
        }
        if (!(m_flags & CustomIconSource)) {
            connect(m_action, &UCAction::iconSourceChanged,
                    this, &UCActionItem::iconSourceChanged, Qt::DirectConnection);
        }
        if (!(m_flags & CustomIconName)) {
            connect(m_action, &UCAction::iconNameChanged,
                    this, &UCActionItem::iconNameChanged, Qt::DirectConnection);
        }
    } else {
        disconnect(this, SIGNAL(triggered(QVariant)),
                   m_action, SLOT(trigger(QVariant)));
        disconnect(m_action, &UCAction::visibleChanged,
                   this, &UCActionItem::_q_updateVisible);
        disconnect(m_action, &UCAction::enabledChanged,
                   this, &UCActionItem::_q_updateEnabled);
        if (!(m_flags & CustomText)) {
            disconnect(m_action, &UCAction::textChanged,
                       this, &UCActionItem::textChanged);
        }
        if (!(m_flags & CustomIconSource)) {
            disconnect(m_action, &UCAction::iconSourceChanged,
                       this, &UCActionItem::iconSourceChanged);
        }
        if (!(m_flags & CustomIconName)) {
            disconnect(m_action, &UCAction::iconNameChanged,
                       this, &UCActionItem::iconNameChanged);
        }
    }
}

/*!
 * \qmlproperty Action ActionItem::action
 * The \l Action associated with this ActionItem. If action is set, the values
 * of the \l Action properties are copied to the values of the ActionItem
 * properties, unless those were previously overridden.
 */
void UCActionItem::setAction(UCAction *action)
{
    if (m_action == action) {
        return;
    }
    if (m_action) {
        attachAction(false);
    }
    m_action = action;
    Q_EMIT actionChanged();

    if (m_action) {
        attachAction(true);
    }
    _q_updateVisible();
    _q_updateEnabled();
    updateProperties();
}

/*!
 * \qmlproperty string ActionItem::text
 * The title of the actionItem. Defaults to the \c action.text.
 */
QString UCActionItem::text()
{
    if (m_flags & CustomText) {
        return m_text;
    }
    return m_action ? m_action->m_text : QString();
}
void UCActionItem::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }
    m_text = text;
    if (m_action && !(m_flags & CustomText)) {
        // disconnect change signal from Action
        disconnect(m_action, &UCAction::textChanged,
                   this, &UCActionItem::textChanged);
    }
    m_flags |= CustomText;
    Q_EMIT textChanged();
}
void UCActionItem::resetText()
{
    m_text.clear();
    m_flags &= ~CustomText;
    if (m_action) {
        // re-connect change signal from Action
        connect(m_action, &UCAction::textChanged,
                this, &UCActionItem::textChanged, Qt::DirectConnection);
    }
    Q_EMIT textChanged();
}

/*!
 * \qmlproperty url ActionItem::iconSource
 * The image associated with the ActionItem. Defaults to \c action.iconSource.
 *
 * This is the URL of any image file. If both iconSource and \l iconName are defined,
 * \l iconName will be ignored.
 */
QUrl UCActionItem::iconSource()
{
    if (m_flags & CustomIconSource) {
        return m_iconSource;
    }
    if (m_action) {
        return m_action->m_iconSource;
    }
    return !iconName().isEmpty() ? QUrl(QString("image://theme/%1").arg(iconName())) : QUrl();
}
void UCActionItem::setIconSource(const QUrl &iconSource)
{
    if (m_iconSource == iconSource) {
        return;
    }
    m_iconSource = iconSource;
    if (m_action && !(m_flags & CustomIconSource)) {
        // disconnect change signal from Action
        disconnect(m_action, &UCAction::iconSourceChanged,
                   this, &UCActionItem::iconSourceChanged);
    }
    m_flags |= CustomIconSource;
    Q_EMIT iconSourceChanged();
}
void UCActionItem::resetIconSource()
{
    m_iconSource.clear();
    m_flags &= ~CustomIconSource;
    if (m_action) {
        // re-connect change signal from Action
        connect(m_action, &UCAction::iconSourceChanged,
                this, &UCActionItem::iconSourceChanged, Qt::DirectConnection);
    }
    Q_EMIT iconSourceChanged();
}

/*!
 * \qmlproperty string ActionItem::iconName
 * The icon associated with the actionItem in the suru icon theme. Defaults to
 * \c action.iconName.
 *
 * \note The complete list of icons available in Ubuntu is not published yet.
 * For now please refer to the folders where the icon themes are installed:
 * \list
 *  \li Ubuntu Touch: \l file:/usr/share/icons/suru
 *  \li Ubuntu Desktop: \l file:/usr/share/icons/ubuntu-mono-dark
 * \endlist
 * These 2 separate icon themes will be merged soon.
 *
 * If both \l iconSource and iconName are defined, iconName will be ignored.
 */
QString UCActionItem::iconName()
{
    if (m_flags & CustomIconName) {
        return m_iconName;
    }
    return m_action ? m_action->m_iconName : QString();
}
void UCActionItem::setIconName(const QString &iconName)
{
    if (m_iconName == iconName) {
        return;
    }
    m_iconName = iconName;
    if (m_action && !(m_flags & CustomIconName)) {
        // disconnect change signal from Action
        disconnect(m_action, &UCAction::iconNameChanged,
                   this, &UCActionItem::iconNameChanged);
    }
    m_flags |= CustomIconName;
    Q_EMIT iconNameChanged();
    // also sync iconSource if that is not a custom one or taken from action
    if (!m_action || (m_flags & CustomIconSource)) {
        Q_EMIT iconSourceChanged();
    }
}
void UCActionItem::resetIconName()
{
    m_iconName.clear();
    m_flags &= ~CustomIconName;
    if (m_action) {
        // re-connect change signal from Action
        connect(m_action, &UCAction::iconNameChanged,
                this, &UCActionItem::iconNameChanged, Qt::DirectConnection);
    }
    Q_EMIT iconNameChanged();
}

/*!
 * \qmlmethod void ActionItem::trigger(var value)
 * Trigger this action item if it is enabled.
 */
void UCActionItem::trigger(const QVariant &value)
{
    if (isEnabled()) {
        Q_EMIT triggered(value);
    }
}
