/*
 * Copyright 2014-2015 Canonical Ltd.
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
#include <QtQml/QQmlEngine>

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
 * as it loads the style only when needed and not upon component creation.
 */
UCListItemStyle::UCListItemStyle(QQuickItem *parent)
    : QQuickItem(parent)
{
}

/*!
 * \qmlsignal ListItemStyle::swipeEvent(SwipeEvent event)
 * The signal is emitted when the ListItem performs swipe related actions, i.e.
 * when the swipe is started, the position is updated and the swipe ends. The
 * \b event object provides information about the swipe status, positions and
 * the updated \l {ListItem::contentItem}{ListItem.contentItem} position. The
 * style implementation can override the contentItem position by setting the
 * \c event.content.x or \c event.content.y properties to the desired value.
 *
 * The \c event object properties are:
 * \list
 * \li * \c status - enumeration of \c {Start, Update, Stop} values representing the
 *                  swipe event status
 * \li * \c mouse - (x, y) coordinates of the current mouse/touch point - read-only
 * \li * \c last - (x, y) coordinates of the previous mouse/touch point - read-only
 * \li * \c content - (x, y) updated coordinates of the \l {ListItem::contentItem}
 *                  {ListItem.contentItem}, read-write
 * \endlist
 */

/*!
 * \qmlsignal ListItemStyle::rebound()
 * Signal emitted by the ListItem when a rebounding action is requested from the
 * style. This usually happens when the list item's content is swiped and there is
 * a press event happening outside of the ListItem's boundary or when the view
 * embedding the ListItem starts scrolling.
 */
