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
    id: dragHandler
    objectName: "draghandler_panel" + index
    width: units.gu(5)

    anchors {
        right: ListItem.item.right
        top: ListItem.item.contentItem.top
        bottom: ListItem.item.contentItem.bottom
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
            target: dragHandler.ListItem.item.contentItem
            anchors.rightMargin: dragHandler.width
        }
        PropertyChanges {
            target: dragIcon
            opacity: 1.0
            scale: 1.0
        }
    }

    transitions: Transition {
        from: ""
        to: "enabled"
        enabled: dragHandler.ListItem.animate
        reversible: true
        SequentialAnimation {
            UbuntuNumberAnimation {
                target: dragHandler.ListItem.item.contentItem
                property: "anchors.rightMargin"
                duration: 50
            }
            PropertyAnimation {
                target: dragIcon
                properties: "opacity,scale"
                easing: UbuntuAnimation.StandardEasing
                duration: UbuntuAnimation.FastDuration
            }
        }
    }

    // make sure the state is changed only after component completion
    Component.onCompleted: {
        state = Qt.binding(function () {
            return ListItem.item.draggable ? "enabled" : "";
        });
    }
}
