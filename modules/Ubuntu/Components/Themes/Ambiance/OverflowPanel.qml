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
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.0 as ListItem

/*!
  \internal
 */
Popover {
    id: overflow

    /*!
      The background color of the tabs panel and the actions overflow panel.
     */
    property color backgroundColor: styledItem.panelColor

    /*!
      The background color of the tapped item in the panel.
     */
    property color highlightColor: Theme.palette.selected.background

    /*!
      The foreground color (icon and text) of actions in the panel.
     */
    property color foregroundColor: Theme.palette.selected.backgroundText

    property bool square: true
    callerMargin: -units.gu(1) + units.dp(4)
    contentWidth: units.gu(20)

    /*!
      False implies the model is a list of Actions.
      True implies the model is a ListModel with a 'tab' role,
      and false implies that the model is a list of actions.
    */
    property bool tabsOverflow: false
    // FIXME: In the input, generate a list of actions from the Tabs
    //  so that we no longer need to make a distinction between the tabs ListModel
    //  and a list of Actions.
    property var model: null

    Binding {
        target: overflow.__foreground.__styleInstance
        property: "color"
        value: overflow.backgroundColor
        when: overflow.__foreground &&
              overflow.__foreground.__styleInstance
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        Repeater {
            id: overflowRepeater
            model: overflow.model
            AbstractButton {
                action: overflow.tabsOverflow ? tab.__protected.action
                                              : modelData

                // These objectNames are used in the CPOs for header and tabs.
                objectName: overflow.tabsOverflow ?
                                "tabButton" + index :
                                action.objectName + "_header_overflow_button"

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
                    visible: !overflow.tabsOverflow
                    source: action.iconSource
                    color: overflow.foregroundColor
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: units.dp(-1)
                        left: parent.left
                        leftMargin: units.gu(2)
                    }
                    width: units.gu(2)
                    height: units.gu(2)
                    opacity: action.enabled ? 1.0 : 0.5
                }

                Label {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: units.dp(-1)
                        left: actionIcon.visible ? actionIcon.right : parent.left
                        leftMargin: units.gu(2)
                        right: parent.right
                    }
                    fontSize: overflow.tabsOverflow ? "medium" : "small"
                    elide: Text.ElideRight
                    text: action.text
                    color: overflow.foregroundColor
                    opacity: action.enabled ? 1.0 : 0.5
                }

                ListItem.ThinDivider {
                    id: bottomDividerLine
                    anchors.bottom: parent.bottom
                    visible: index !== overflowRepeater.count - 1
                }
            }
        }
    }
}
