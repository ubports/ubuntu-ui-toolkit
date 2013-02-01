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

/*!
    \internal
    \qmltype GenericToolbar
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
*/
// FIXME: This class is going to be deprecated when we use
//  the toolbar behavior from the shell.
Item {
    id: bottomBar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
//    clip: true

    /*!
      When active, the bar is visible, otherwise it is hidden.
      Use bottom edge swipe up/down to activate/deactivate the bar.
     */
    property bool active: false
    onActiveChanged: bar.updateYPosition();

    default property alias contents: bar.data

    Item {
        id: bar
        height: parent.height
        anchors {
            left: parent.left
            right: parent.right
        }

        // initial state only. Will be overridden because of mouseArea's drag target.
        y: bottomBar.active ? 0 : height

        function updateYPosition() {
            if (bottomBar.active) bar.y = 0;
            else bar.y = bar.height;
        }

//        property bool notAnimating: (bottomBar.active && y === 0) || (!bottomBar.active && y === height)
        Behavior on y {
            SmoothedAnimation {
                velocity: 500;
                easing.type: Easing.InOutQuad;
            }
        }
    }

    DraggingArea {
        // visible: toolbar.available
        orientation: Qt.Vertical
        id: dragMouseArea
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: bottomBar.active ? bar.height : units.gu(3)
        zeroVelocityCounts: true

        drag {
            target: bar
            axis: Drag.YAxis
            minimumY: 0
            //maximumY: height + bar.height
        }

        propagateComposedEvents: true

        // FIXME: Make all parameters below themable.
        //  The value of 44 was copied from the Launcher.
        onPressedChanged: {
            if (pressed) {
                if (bottomBar.active) return;
                y = height - units.gu(1);
            } else {
                if (dragMouseArea.dragVelocity < -44) {
                    bottomBar.active = true;
                } else if (dragMouseArea.dragVelocity > 44) {
                    bottomBar.active = false;
                } else {
                    bottomBar.active = bar.y < bar.height / 2;
                }
                bar.updateYPosition();
            }
        }
    }
}
