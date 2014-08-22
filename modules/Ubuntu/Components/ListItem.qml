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
    \ingroup ubuntu
    \brief The ListItem element sets up the sizes and other bindings to satisfy
    Ubuntu design guides. Does not bring anything in addition to ListItemBase.
  */

ListItemBase {
    width: owningItem ? owningItem.width : units.gu(20)
    height: units.gu(6)
    background.pressedColor: Theme.palette.selected.background
    divider {
        leftMargin: units.gu(2)
        rightMargin: units.gu(2)
    }
}
