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
import Ubuntu.Components 1.2

/*
  This component is the holder of the ListItem selection checkbox.
  */
Item {
    id: panel
    width: height

    /*
      Internally used to link to the list item's content
      */
    readonly property Item contentItem: parent ? parent.contentItem : null

    /*
      Exposed checked property linked to the listItem's selected
      */
    property alias checked: checkbox.checked

    anchors {
        right: contentItem ? contentItem.left : undefined
        top: contentItem ? contentItem.top : undefined
        bottom: contentItem ? contentItem.bottom : undefined
    }

    CheckBox {
        id: checkbox
        anchors.centerIn: parent
    }
}
