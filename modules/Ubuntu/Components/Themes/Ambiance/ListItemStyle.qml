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

import QtQuick 2.3
import Ubuntu.Components.Styles 1.2 as Styles
import Ubuntu.Components 1.2

Styles.ListItemStyle {

    actionsDelegate: ListItemPanel{}

    snapAnimation: PropertyAnimation {
        property: "x"
        easing {
            type: Easing.OutElastic
            period: 0.5
        }
        duration: UbuntuAnimation.BriskDuration
        alwaysRunToEnd: true
    }

    // the selection/multiselection panel
    selectionDelegate: Item {
        id: selectionPanel
        width: checkbox.width + 2 * units.gu(2)
        /*
          Set if the ListItem is in selection mode
          */
        readonly property bool inSelectionMode: parent ? parent.selectable : false

        /*
          Internally used to link to the list item's content. The parent item is the ListItem itself.
          */
        readonly property Item contentItem: parent ? parent.contentItem : null

        anchors {
            right: contentItem ? contentItem.left : undefined
            top: contentItem ? contentItem.top : undefined
            bottom: contentItem ? contentItem.bottom : undefined
        }

        states: State {
            name: "enabled"
            PropertyChanges {
                target: selectionPanel.parent.contentItem
                x: selectionPanel.width
            }
        }

        transitions: Transition {
            from: ""
            to: "enabled"
            reversible: true
            PropertyAnimation {
                target: selectionPanel.parent.contentItem
                property: "x"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }

        state: inSelectionMode ? "enabled" : ""

        CheckBox {
            id: checkbox
            // for unit and autopilot tests
            objectName: "listitem_select"
            anchors.centerIn: parent
            checked: selectionPanel.parent ? selectionPanel.parent.selected : false

            Binding {
                target: selectionPanel.parent
                property: "selected"
                value: checkbox.checked
                when: selectionPanel.parent
            }
        }
    }
}
