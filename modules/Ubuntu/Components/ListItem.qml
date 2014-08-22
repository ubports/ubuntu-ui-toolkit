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

import QtQuick 2.2
import Ubuntu.Components 1.1

/*!
    \qmltype ListItem
    \inqmlmodule Ubuntu.Components 1.1
    \inherits Item
    \ingroup ubuntu
    \brief The ListItem element provides Ubuntu design standards for list or grid
    views.

    The component is dedicated to be used in designs with static or dynamic lists
    (i.e. list views where each item's layout differs or in lists where the content
    is determined by a given model, thus each element has the same layout). The
    element does not define any specific layout, components can be placed in any
    ways on it. However, when used in list views, the content must be carefully
    chosen to in order to keep the kinetic behavior and the highest FPS if possible.
  */

ListItemBase {
    width: owningItem ? owningItem.width : units.gu(20)
    height: units.gu(6)
    background.pressedColor: Theme.palette.selected.background
}
