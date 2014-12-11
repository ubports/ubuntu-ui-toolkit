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

    swipeOvershoot: units.gu(2)
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
        objectName: "selection_panel"
        width: units.gu(5)

        readonly property ListItem listItem: parent

        /*
          Set if the ListItem is selected
          */
        readonly property bool selected: listItem ? listItem.selected : false

        /*
          Internally used to link to the list item's content. The parent item is the ListItem itself.
          */
        readonly property Item contentItem: listItem ? listItem.contentItem : null

        anchors {
            right: contentItem ? contentItem.left : undefined
            top: contentItem ? contentItem.top : undefined
            bottom: contentItem ? contentItem.bottom : undefined
        }

        states: State {
            name: "enabled"
            PropertyChanges {
                target: contentItem
                x: selectionPanel.width
                width: listItem.width - selectionPanel.width - (listItem.draggable ? units.gu(5) : 0)
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
            ParallelAnimation {
                PropertyAnimation {
                    target: selectionPanel.parent.contentItem
                    properties: "x,width"
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
                return listItem && listItem.selectable ? "enabled" : "";
            });
        }

        CheckBox {
            id: checkbox
            // for unit and autopilot tests
            objectName: "listitem_select"
            anchors.centerIn: parent
            opacity: 0.0
            // for the initial value
            checked: selectionPanel.selected
            onCheckedChanged: {
                if (selectionPanel.parent) {
                    selectionPanel.parent.selected = checked;
                }
            }
        }
        onSelectedChanged: checkbox.checked = selected
    }

    dragHandlerDelegate: Item {
        id: dragHandler
        objectName: "draghandler_panel" + index
        width: units.gu(5)

        readonly property ListItem listItem: parent
        /*
          Internally used to link to the list item's content. The parent item is the ListItem itself.
          */
        readonly property Item contentItem: listItem ? listItem.contentItem : null

        anchors {
            right: listItem ? listItem.right : undefined
            top: contentItem ? contentItem.top : undefined
            bottom: contentItem ? contentItem.bottom : undefined
        }

        Icon {
            objectName: "icon"
            id: dragIcon
            anchors.centerIn: parent
            width: units.gu(3)
            height: width
            name: "view-grid-symbolic"
            opacity: 0.0
            scale: 0.5
        }

        states: State {
            name: "enabled"
            PropertyChanges {
                target: dragIcon
                opacity: 1.0
                scale: 1.0
            }
            PropertyChanges {
                target: contentItem
                width: listItem.width - dragHandler.width - (listItem.selectable ? units.gu(5) : 0)
            }
        }

        transitions: Transition {
            from: ""
            to: "enabled"
            reversible: true
            PropertyAnimation {
                target: dragIcon
                properties: "opacity,scale"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }

        // make sure the state is changed only after component completion
        Component.onCompleted: {
            state = Qt.binding(function () {
                return listItem && listItem.draggable ? "enabled" : "";
            });
        }
    }
}
