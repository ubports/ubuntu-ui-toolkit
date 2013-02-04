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
Item {
    id: bottomBar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }

    /*!
      When active, the bar is visible, otherwise it is hidden.
      Use bottom edge swipe up/down to activate/deactivate the bar.
     */
//    property bool active: false
//    onActiveChanged: bar.updateYPosition();

    default property alias contents: bar.data

    /*!
      How much of the toolbar to show when starting interaction"
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
                y: MathUtils.clamp(bar.height, dragMouseArea.mouseY - internal.movingDelta, 0, bar.height)
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

    QtObject {
        id: internal
        property string previousState: ""
        property int movingDelta
        onMovingDeltaChanged: print("movingDelta = "+movingDelta)
    }

    onStateChanged: {
        print("state = "+state);
        if (state == "hint") {
            internal.movingDelta = bottomBar.hintSize + dragMouseArea.initialY - bar.height;
        } else if (state == "moving" && internal.previousState == "spread") {
            internal.movingDelta = dragMouseArea.initialY;
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

        // initial state only. Will be overridden because of mouseArea's drag target.
//        y: bottomBar.active ? 0 : height
        y: height

//        function updateYPosition() {
//            if (bottomBar.active) bar.y = 0;
//            else bar.y = bar.height;
//        }

//        Behavior on y {
//            SmoothedAnimation {
//                velocity: 500;
//                easing.type: Easing.InOutQuad;
//            }
//        }
    }

    DraggingArea {
        orientation: Qt.Vertical
        id: dragMouseArea
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: bar.height //bottomBar.active ? bar.height : units.gu(3)
        zeroVelocityCounts: true

        property int initialY

        propagateComposedEvents: true

        onPressed: {
            initialY = mouseY;
            print("initialY = "+initialY);
            if (bottomBar.state == "") bottomBar.state = "hint";
            else bottomBar.state = "moving";
        }

        onPositionChanged: {
            if (bottomBar.state == "hint" && mouseY < initialY) {
                bottomBar.state = "moving";
            }
            // detect moving out of the window without releasing on desktop
//            print(bar.height - mouseY);
        }


//        onExited: released(mouse)
        onExited: print("EXIT!")
        // FIXME: Make all parameters below themable.
        //  The value of 44 was copied from the Launcher.
        onReleased: finishMoving()

        function finishMoving() {
            if (dragMouseArea.dragVelocity < -44) {
                bottomBar.state = "spread";
            } else if (dragMouseArea.dragVelocity > 44) {
                bottomBar.state = "";
            } else {
                bottomBar.state = (bar.y < bar.height / 2) ? "spread" : "";
            }
        }

        // Mouse cursor moving out of the window while pressed on desktop
        onCanceled: finishMoving()
//        onPressedChanged: {
//            if (pressed) {
//                if (bottomBar.active) return;
//                //bar.y = bar.height - bottomBar.hintSize;
//            } else {
//                if (dragMouseArea.dragVelocity < -44) {
//                    bottomBar.active = true;
//                } else if (dragMouseArea.dragVelocity > 44) {
//                    bottomBar.active = false;
//                } else {
//                    bottomBar.active = bar.y < bar.height / 2;
//                }
//                bar.updateYPosition();
//            }
//        }
    }
}
