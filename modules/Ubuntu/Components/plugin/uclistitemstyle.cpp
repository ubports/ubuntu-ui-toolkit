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

#include "uclistitemstyle.h"

/*!
 * \qmltype ListItemStyle
 * \instantiates UCListItemStyle
 * \inqmlmodule Ubuntu.Components.Styles 1.2
 * \ingroup style-api
 * \since Ubuntu.Components.Styles 1.2
 * \brief Style API for ListItem component.
 *
 * The component defines the style API for the ListItem component. Provides the
 * default actions visualization panel, snap animation, selection mode and drag
 * handler delegates. ListItem treats the style differently compared to the other
 * components, as it will load it only when needed, to load the components defining
 * the look and feel of the different configurable elements of the component,
 * and will ignore any visuals declared in the style.
 */
UCListItemStyle::UCListItemStyle(QQuickItem *parent) :
    QQuickItem(parent)
{
}

/*!
 * \qmlproperty Component ListItemStyle::actionsDelegate
 * Specifies the component visualizing the leading/trailing actions.
 */
void UCListItemStyle::setActionsDelegate(QQmlComponent *delegate)
{
    if (m_actionsDelegate == delegate) {
        return;
    }
    m_actionsDelegate = delegate;
    Q_EMIT actionsDelegateChanged();
}

/*!
 * \qmlproperty Component ListItemStyle::selectionDelegate
 * Holds the component handling the selection mode.
 */
void UCListItemStyle::setSelectionDelegate(QQmlComponent *delegate)
{
    if (m_selectionDelegate == delegate) {
        return;
    }
    m_selectionDelegate = delegate;
    Q_EMIT selectionDelegateChanged();
}

/*!
 * \qmlproperty Component ListItemStyle::dragHandlerDelegate
 * Holds the component shown when dragging mode is enabled.
 */
void UCListItemStyle::setDragHandlerDelegate(QQmlComponent *delegate)
{
    if (m_dragHandlerDelegate == delegate) {
        return;
    }
    m_dragHandlerDelegate = delegate;
    Q_EMIT dragHandlerDelegateChanged();
}

/*!
 * \qmlproperty PropertyAnimation ListItemStyle::snapAnimation
 * Holds the animation used in animating when snapped in or out.
 */
void UCListItemStyle::setSnapAnimation(QQuickPropertyAnimation *animation)
{
    if (m_snapAnimation == animation) {
        return;
    }
    m_snapAnimation = animation;
    Q_EMIT snapAnimationChanged();
}
