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
  This component is the holder of the ListItem options.
  */
Item {
    id: panel
    width: units.gu(20)

    // for testing
    objectName: "ListItemPanel" + (leadingPanel ? "Leading" : "Trailing")

    /*
      Property holding the ListItem's contentItem instance
      */
    readonly property Item contentItem: parent ? parent.contentItem : null
    /*
      Specifies whether the panel is used to visualize leading or trailing options.
      */
    property bool leadingPanel: panel.ListItemActions.status == panel.ListItemActions.Leading
    /*
      The delegate to be used to visualize the options
      */
    property Component delegate

    anchors {
        left: contentItem ? (leadingPanel ? undefined : contentItem.right) : undefined
        right: contentItem ? (leadingPanel ? contentItem.left : undefined) : undefined
        top: contentItem ? contentItem.top : undefined
        bottom: contentItem ? contentItem.bottom : undefined
    }

    Rectangle {
        objectName: "panel_background"
        anchors {
            fill: parent
            // add 4 times the overshoot margins to cover the background when tugged
            leftMargin: leadingPanel ? -units.gu(4 * panel.ListItemActions.overshoot) : 0
            rightMargin: leadingPanel ? 0 : -units.gu(4 * panel.ListItemActions.overshoot)
        }
        // FIXME: use Palette colors instead when available
        color: (leadingPanel ? UbuntuColors.red : "white")
    }
}
