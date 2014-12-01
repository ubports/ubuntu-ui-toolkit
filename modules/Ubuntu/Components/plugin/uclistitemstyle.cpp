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
 * Style API for the ListItem component which provides actions, select and
 * drag handler delegates, and snap animation via its properties.
 * ListItem treats the style differently compared to the other components,
 * as it:
 * \list
 *  \li - loads the style only when needed
 *  \li - gets delegates and snap animation from the style properties
 *  \li - ignores any other visuals defined in the style.
 * \endlist
 */
UCListItemStyle::UCListItemStyle(QQuickItem *parent)
    : QQuickItem(parent)
    , m_actionsDelegate(0)
    , m_selectionDelegate(0)
    , m_dragHandlerDelegate(0)
    , m_snapAnimation(0)
    , m_swipeOvershoot(0)
{
}

/*!
 * \qmlproperty Component ListItemStyle::actionsDelegate
 * Specifies the component visualizing the leading/trailing actions.
 */

/*!
 * \qmlproperty Component ListItemStyle::selectionDelegate
 * Holds the component handling the selection mode. The component is responsible
 * to handle the visualization of the component, including the changes to be
 * applied over the ListItem visuals i.e. pushing the \l {ListItem::contentItem}
 * {contentItem}, animating the changes, etc. ListItem will set the visible property
 * of this component to true when selectable, and false when the component is removed.
 */

/*!
 * \qmlproperty Component ListItemStyle::dragHandlerDelegate
 * Holds the component shown when dragging mode is enabled.
 */

/*!
 * \qmlproperty PropertyAnimation ListItemStyle::snapAnimation
 * Holds the animation used in animating when snapped in or out.
 */

/*!
 * \qmlproperty real ListItemStyle::swipeOvershoot
 * The property specifies the overshoot value of the ListItem.
 */
