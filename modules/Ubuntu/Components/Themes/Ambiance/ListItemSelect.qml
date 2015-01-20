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

import QtQuick 2.3
import Ubuntu.Components 1.2

Item {
    id: selectionPanel
    objectName: "selection_panel" + ListItem.index
    width: units.gu(5)

    anchors {
        right: ListItem.item.contentItem.left
        top: ListItem.item.contentItem.top
        bottom: ListItem.item.contentItem.bottom
    }

    states: State {
        name: "enabled"
        PropertyChanges {
            target: selectionPanel.ListItem.item.contentItem
            x: selectionPanel.width
            anchors.leftMargin: selectionPanel.width
        }
        PropertyChanges {
            target: checkbox
            opacity: 1.0
        }
    }

    transitions: Transition {
        from: ""
        to: "enabled"
        reversible: true
        enabled: selectionPanel.ListItem.animate
        ParallelAnimation {
            PropertyAnimation {
                target: selectionPanel.ListItem.item.contentItem
                properties: "x,anchors.leftMargin"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
            PropertyAnimation {
                target: checkbox
                property: "opacity"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }
    }

    // make sure the state is changed only after component completion
    Component.onCompleted: {
        state = Qt.binding(function () {
            return ListItem.item.selectable ? "enabled" : "";
        });
    }

    CheckBox {
        id: checkbox
        // for unit and autopilot tests
        objectName: "listitem_select"
        anchors.centerIn: parent
        opacity: 0.0
        // for the initial value
        checked: selectionPanel.ListItem.item.selected
        onCheckedChanged: {
            if (selectionPanel.ListItem.item) {
                selectionPanel.ListItem.item.selected = checked;
            }
        }
    }
    // update checkbox when ViewItems.selectionIndices changes
    Connections {
        target: selectionPanel.ListItem.item
        onSelectedChanged: checkbox.checked = selectionPanel.ListItem.item.selected
    }
}
