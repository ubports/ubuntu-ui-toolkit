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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \internal
    \qmltype GenericToolbar
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
*/
Item {
    id: bottomBar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    default property alias contents: bar.data

    /*!
      When active, the bar is visible, otherwise it is hidden.
      Use bottom edge swipe up/down to activate/deactivate the bar.
      The active property is not updated until the swipe gesture is completed.
     */
    property bool active: false
    onActiveChanged: {
        if (active) state = "spread";
        else state = "";
    }

    /*!
      Disable bottom edge swipe to activate/deactivate the toolbar.
     */
    property bool lock: false
    onLockChanged: {
        if (state == "hint" || state == "moving") {
            draggingArea.finishMoving();
        }
    }

    /*!
      How much of the toolbar to show when starting interaction.
     */
    property real hintSize: units.gu(1)

    states: [
        State {
            name: "hint"
            PropertyChanges {
                target: bar
                y: bar.height - bottomBar.hintSize
            }
        },
        State {
            name: "moving"
            PropertyChanges {
                target: bar
                y: MathUtils.clamp(bar.height, draggingArea.mouseY - internal.movingDelta, 0, bar.height)
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: bar
                y: 0
            }
        },
        State {
            name: ""
            PropertyChanges {
                target: bar
                y: bar.height
            }
        }
    ]

    transitions: [
        Transition {
            to: ""
            PropertyAnimation {
                target: bar
                properties: "y"
                duration: 100
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "hint"
            PropertyAnimation {
                target: bar
                properties: "y"
                duration: 100
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "spread"
            PropertyAnimation {
                target: bar
                properties: "y"
                duration: 100
                easing.type: Easing.OutQuad
            }
        }
    ]

    QtObject {
        id: internal
        property string previousState: ""
        property int movingDelta

        // Used for recovering the state from before
        //  bottomBarVisibilityCommunicator forced the toolbar to hide.
        property bool savedLock: bottomBar.lock
        property bool savedActive: bottomBar.active
    }

    Connections {
        target: bottomBarVisibilityCommunicator
        onForceHiddenChanged: {
            if (bottomBarVisibilityCommunicator.forceHidden) {
                internal.savedLock = bottomBar.lock;
                internal.savedActive = bottomBar.active;
                bottomBar.active = false;
                bottomBar.lock = true;
            } else { // don't force hidden
                bottomBar.active = internal.savedActive;
                bottomBar.lock = internal.savedLock;
            }
        }
    }

    Component.onCompleted: {
        bottomBarVisibilityCommunicator.targetHeight = toolbar.height;
    }

    onStateChanged: {
        if (state == "hint") {
            internal.movingDelta = bottomBar.hintSize + draggingArea.initialY - bar.height;
        } else if (state == "moving" && internal.previousState == "spread") {
            internal.movingDelta = draggingArea.initialY;
        } else if (state == "spread") {
            bottomBar.active = true;
        } else if (state == "") {
            bottomBar.active = false;
        }
        internal.previousState = state;
    }

    Item {
        id: bar
        height: parent.height
        anchors {
            left: parent.left
            right: parent.right
        }

        y: bottomBar.active ? 0 : height
    }

    DraggingArea {
        orientation: Qt.Vertical
        id: draggingArea
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: bottomBar.active ? bar.height + units.gu(1) : units.gu(3)
        zeroVelocityCounts: true
        propagateComposedEvents: true
        visible: !bottomBar.lock

        property int initialY
        onPressed: {
            initialY = mouseY;
            if (bottomBar.state == "") bottomBar.state = "hint";
            else bottomBar.state = "moving";
        }

        onPositionChanged: {
            if (bottomBar.state == "hint" && mouseY < initialY) {
                bottomBar.state = "moving";
            }
        }

        onReleased: finishMoving()
        // Mouse cursor moving out of the window while pressed on desktop
        onCanceled: finishMoving()

        // FIXME: Make all parameters below themable.
        //  The value of 44 was copied from the Launcher.
        function finishMoving() {
            if (draggingArea.dragVelocity < -44) {
                bottomBar.state = "spread";
            } else if (draggingArea.dragVelocity > 44) {
                bottomBar.state = "";
            } else {
                bottomBar.state = (bar.y < bar.height / 2) ? "spread" : "";
            }
        }
    }
}
