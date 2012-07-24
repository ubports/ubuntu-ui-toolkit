/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 1.1

/*!
  \qmlclass Selectable
  \inqmlmodule UbuntuUIToolkit 0.1
  \brief A base list item that has a selected state.

  This class is inherited by several types of selectable list items
  and should not be used directly.
 */
Base {
    id: selectableListIem

    /*!
      \preliminary
      Specifies whether the list item is selected.
     */
    property bool selected: false

    Image {
        visible: selectableListIem.selected
        anchors {
            top: parent.top
            topMargin: 2
            right: parent.right
            rightMargin: 2
        }
        source: "artwork/list_item_selected_triangle.png"
    }
}
