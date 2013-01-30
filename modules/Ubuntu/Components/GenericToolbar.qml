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
    id: chromeBar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    height: units.gu(10)

    clip: true

    /*!
      When active, the bar is visible, otherwise it is hidden.
      Use bottom edge swipe up/down to activate/deactivate the bar.
     */
    property bool active: false

    default property alias contents: bar.data

    Item {
        id: bar

        height: parent.height
        anchors {
            left: parent.left
            right: parent.right
        }
        y: chromeBar.active ? 0 : height

        property bool notAnimating: (chromeBar.active && y === 0) || (!chromeBar.active && y === height)
        Behavior on y {
            NumberAnimation {
                duration: 200;
                easing.type: Easing.InOutQuad;
            }
        }
    }

    MouseArea {
        anchors.fill: parent

        // avoid propagating events when bar in the process
        // of becoming active or inactive.
        propagateComposedEvents: bar.notAnimating

        /*!
          The amount that the cursor position needs to change in y-direction
          after pressing, in order to activate/deactivate the bar.
         */
        property real dragThreshold: units.gu(1)

        property int pressedY
        onPressed: {
            pressedY = mouse.y;
            mouse.accepted = true;
        }

        onPositionChanged: {
            var diff = pressedY - mouse.y;
            if (diff > dragThreshold) {
                chromeBar.active = true;
            } else if (diff < -dragThreshold) {
                chromeBar.active = false;
            }
        }
    }
}
