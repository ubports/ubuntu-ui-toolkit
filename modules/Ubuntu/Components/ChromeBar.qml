/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.0
//import Ubuntu.Components 0.1
//import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    id: chromeBar

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    height: units.gu(6) // TODO: make themable

    property alias showChromeBar: bar.shown

    default property alias contents: bar.data
//    onEnabledChanged: {
//        if (!enabled) {
//            setBarShown(false);
//        }
//    }

//    Component.onCompleted: setBarShown(false)

    // do not allow hiding of the toolbar
    property bool alwaysVisible: false
    onAlwaysVisibleChanged: setBarShown(alwaysVisible)

    function setBarShown(shown) {
        if (shown) {
            bar.y = 0;
        } else {
            bar.y = bar.height;
        }
        bar.shown = shown;
    }

    MouseArea {
        anchors.fill: parent
//        enabled: !chromeBar.alwaysVisible
        propagateComposedEvents: true

        property int __pressedY
        onPressed: {
            __pressedY = mouse.y;
            mouse.accepted = true;
        }

        onPositionChanged: {
            var diff = __pressedY - mouse.y;
            if (diff > dragThreshold) setBarShown(true);
            else if (diff < -dragThreshold) setBarShown(false);
        }

        property real dragThreshold: units.dp(2) // TODO: make themable?

        Rectangle {
            id: touchAreaForDebugging
            color: "grey"
            opacity: 0.1
            anchors.fill: parent
        }

        Item {
            id: bar

            property bool shown: false
            height: units.gu(6) //+ orangeRect.height
            anchors.left: parent.left
            anchors.right: parent.right
            y: parent.height

            Rectangle {
                id: background
                anchors.fill: parent
                color: "white"
            }

            Behavior on y {
                NumberAnimation {
                    duration: 150
                }
            }

//            Item {
//                id: contents
//                anchors.left: parent.left
//                anchors.right: parent.right
//                height: units.gu(8)
//            }
        }
    }
}
