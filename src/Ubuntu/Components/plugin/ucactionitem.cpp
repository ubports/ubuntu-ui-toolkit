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
{
    connect(this, &UCActionItem::visibleChanged, this, &UCActionItem::_q_visibleChanged);
    connect(this, &UCActionItem::enabledChanged, this, &UCActionItem::_q_enabledChanged);
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
void UCActionItem::updateVisible()
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
void UCActionItem::updateEnabled()
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
        disconnect(this, &UCActionItem::triggered, m_action, &UCAction::triggered);
    }
    m_action = action;
    Q_EMIT actionChanged();

    if (m_action) {
        connect(this, &UCActionItem::triggered, m_action, &UCAction::triggered);
    }
    updateVisible();
    updateEnabled();
    updateProperties();
}

/*!
 * \qmlproperty string ActionItem::text
 * The title of the actionItem. Defaults to the \c action.text.
 */
QString UCActionItem::text()
{
    return (m_action  && !(m_flags & CustomText)) ? m_action->m_text : m_text;
}
void UCActionItem::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }
    m_text = text;
    m_flags |= CustomText;
    Q_EMIT textChanged();
}
void UCActionItem::resetText()
{
    m_text.clear();
    m_flags &= ~CustomText;
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
    m_flags |= CustomIconSource;
    Q_EMIT iconSourceChanged();
}
void UCActionItem::resetIconSource()
{
    m_iconSource.clear();
    m_flags &= ~CustomIconSource;
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
    return (m_action && !(m_flags & CustomIconName)) ? m_action->m_iconName : m_iconName;
}
void UCActionItem::setIconName(const QString &iconName)
{
    if (m_iconName == iconName) {
        return;
    }
    m_iconName = iconName;
    m_flags |= CustomIconName;
    Q_EMIT iconNameChanged();
}
void UCActionItem::resetIconName()
{
    m_iconName.clear();
    m_flags &= ~CustomIconName;
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
