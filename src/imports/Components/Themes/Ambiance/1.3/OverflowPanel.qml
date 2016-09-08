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

import QtQuick 2.4
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

/*!
  \internal
 */
ActionSelectionPopover {
    id: overflow

    // clicks next to the overflow panel must only close the panel,
    // not trigger additional events.
    grabDismissAreaEvents: true

    /*!
      The background color of the tabs panel and the actions overflow panel.
     */
    property color backgroundColor: theme.palette.normal.background

    /*!
      The background color of the tapped item in the panel.
     */
    property color highlightColor: theme.palette.highlighted.background

    /*!
      The foreground color (icon and text) of actions in the panel.
     */
    property color foregroundColor: theme.palette.normal.backgroundText

    /*!
      The foreground color (icon and text) of disabled actions in the panel.
     */
    property color disabledForegroundColor: theme.palette.disabled.backgroundText

    property bool square: true
    callerMargin: -units.gu(1) + units.dp(4)
    contentWidth: units.gu(20)

    Binding {
        target: overflow.__foreground.__styleInstance
        property: "color"
        value: overflow.backgroundColor
        when: overflow.__foreground &&
              overflow.__foreground.__styleInstance
    }

    delegate: AbstractButton {
        id: actionButton
        action: modelData

        // close after triggering the action.
        onClicked: overflow.hide()

        implicitHeight: units.gu(6) + bottomDividerLine.height
        width: parent ? parent.width : units.gu(31)

        Rectangle {
            visible: parent.pressed
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: parent.height - bottomDividerLine.height
            color: overflow.highlightColor
        }

        Icon {
            id: actionIcon
            visible: "" != action.iconSource
            source: action.iconSource
            color: enabled ? overflow.foregroundColor
                           : overflow.disabledForegroundColor
            anchors {
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: units.dp(-1)
                left: parent.left
                leftMargin: units.gu(2)
            }
            width: units.gu(2)
            height: units.gu(2)
        }

        Label {
            anchors {
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: units.dp(-1)
                left: actionIcon.visible ? actionIcon.right : parent.left
                leftMargin: units.gu(2)
                right: parent.right
            }
            // In the tabs overflow panel there are no icons, and the font-size
            //  is medium as opposed to the small font-size in the actions overflow panel.
            textSize: actionIcon.visible ? Label.Small : Label.Medium
            elide: Text.ElideRight
            text: action.text
            color: enabled ? overflow.foregroundColor
                           : overflow.disabledForegroundColor
        }

        // The value of showDivider is automatically set by ActionSelectionPopover.
        property bool showDivider
        ListItem.ThinDivider {
            id: bottomDividerLine
            anchors.bottom: parent.bottom
            visible: actionButton.showDivider
        }
    }
}
