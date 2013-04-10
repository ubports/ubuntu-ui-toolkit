/*
 * Copyright (C) 2013 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 0.1 as Toolkit

/*!
    \qmltype Panel
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu

    TODO: document
    Panel should not be themed, but the contents of the Panel.
    The reason is that the panel itself should not have visible elements, since it is
    in the view (to detect mouse events) even when its contents should be invisible
*/
Item {
    id: panel

    default property alias contents: bar.data

    /*!
      The property defines the alignment of the panel.
      The implementation supports the following values:
        \list
        \li Qt.AlignLeft to swipe in the panel from the left
        \li Qt.AlignRight to swipe in the panel from the right
        \li Qt.AlignTop to swipe in the panel from the top
        \li Qt.AlignBottom to swipe in the panel from the bottom
        \endlist
        The default value is \b Qt.AlignBottom, and it is not recommended to change the
        default value because the left, right and top edge are already used for system
        functions, while the bottom edge is reserved for app-specific functionality
        such as a default toolbar.
      */
    property int align: Qt.AlignBottom

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
      How much of the panel to show when starting interaction.
     */
    property real hintSize: units.gu(2)

    /*!
      The height of the mouse area used to detect edge swipes to
      activate the panel.
     */
    property real triggerSize: units.gu(2)

    states: [
        State {
            name: "hint"
            PropertyChanges {
                target: bar
                v: bar.size - panel.hintSize
            }
        },
        State {
            name: "moving"
            PropertyChanges {
                target: bar
                v: MathUtils.clamp(draggingArea.mouseV - internal.movingDelta, 0, bar.size)
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: bar
                v: 0
            }
        },
        State {
            name: ""
            PropertyChanges {
                target: bar
                v: bar.size
                explicit: true
            }
        }
    ]

    /*!
      The toolbar is currently not in a stable hidden or visible state.
     */
    readonly property bool animating: draggingArea.pressed || (state == "" && bar.v != bar.size) || (state == "spread" && bar.v != 0)

    transitions: [
        Transition {
            to: ""
            PropertyAnimation {
                target: bar
                properties: "v"
                duration: 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "hint"
            PropertyAnimation {
                target: bar
                properties: "v"
                duration: 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "spread"
            PropertyAnimation {
                target: bar
                properties: "v"
                duration: 50
                easing.type: Easing.OutQuad
            }
        }
    ]

    QtObject {
        id: internal
        property string previousState: ""
        property int movingDelta

        // Used for recovering the state from before
        //  panelVisibilityCommunicator forced the toolbar to hide.
        property bool savedLock: panel.lock
        property bool savedActive: panel.active

        readonly property int orientation: (panel.align === Qt.AlignTop || panel.align === Qt.AlignBottom)
                                           ? Qt.Horizontal : Qt.Vertical
    }

    Connections {
        // FIXME: bottomBarVisibilityCommunicator is not the most-suitable name anymore.
        target: bottomBarVisibilityCommunicator
        onForceHiddenChanged: {
            if (bottomBarVisibilityCommunicator.forceHidden) {
                internal.savedLock = panel.lock;
                internal.savedActive = panel.active;
                panel.active = false;
                panel.lock = true;
            } else { // don't force hidden
                panel.lock = internal.savedLock;
                if (internal.savedLock) panel.active = internal.savedActive;
                // if the panel was locked, do not slide it back in
                // until the user performs an edge swipe.
            }
        }
    }

    onStateChanged: {
        if (state == "hint") {
            internal.movingDelta = (panel.hintSize + draggingArea.initialV - bar.size);
        } else if (state == "moving" && internal.previousState == "spread") {
            internal.movingDelta = draggingArea.initialV;
        } else if (state == "spread") {
            panel.active = true;
        } else if (state == "") {
            panel.active = false;
        }
        internal.previousState = state;
    }

    // FIXME: The InverseMouseArea below is not working because of this bug:
    // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1166127
    // Uncomment when the bug is fixed.
//    Toolkit.InverseMouseArea {
//        anchors.fill: draggingArea
//        onClicked: {
//            mouse.accepted = false;
//            // the mouse click may cause an update
//            //  of lock by the clicked Item behind
//            if (!panel.lock) panel.active = false;
//        }
//        propagateComposedEvents: true
//        visible: panel.lock == false && panel.state == "spread"
//    }

    DraggingArea {

        orientation: internal.orientation === Qt.Horizontal ? Qt.Vertical : Qt.Horizontal
        id: draggingArea
        anchors {
            top: panel.align === Qt.AlignBottom ? undefined : parent.top
            bottom: panel.align === Qt.AlignTop ? undefined : parent.bottom
            left: panel.align === Qt.AlignRight ? undefined : parent.left
            right: panel.align === Qt.AlignLeft ? undefined : parent.right
        }
        height: internal.orientation === Qt.Horizontal ? panel.active ? bar.size + units.gu(1) : panel.triggerSize : undefined
        width: internal.orientation === Qt.Vertical ? panel.active ? bar.size + units.gu(1) : panel.triggerSize : undefined

        zeroVelocityCounts: true
        propagateComposedEvents: true
        visible: !panel.lock

        property int mouseV: getMouseV()
        function getMouseV() {
            switch (panel.align) {
            case Qt.AlignLeft:
                return -mouseX;
            case Qt.AlignRight:
                return mouseX;
            case Qt.AlignBottom:
                return mouseY;
            case Qt.AlignTop:
                return -mouseY;
            }
        }

        property int initialV
        onPressed: {
            initialV = getMouseV();
            if (panel.state == "") panel.state = "hint";
        }

        onPositionChanged: {
            if (panel.state == "hint" && mouseV < initialV) {
                panel.state = "moving";
            } else if (panel.state == "spread" && mouseV > initialV) {
                panel.state = "moving";
            }
        }

        onReleased: finishMoving()
        // Mouse cursor moving out of the window while pressed on desktop
        onCanceled: finishMoving()

        // FIXME: Make all parameters below themable.
        //  The value of 44 was copied from the Launcher.
        function finishMoving() {
            if (draggingArea.dragVelocity < -44) {
                if (panel.align === Qt.AlignBottom || panel.align === Qt.AlignRight) {
                    panel.state = "spread";
                } else {
                    panel.state = "";
                }
            } else if (draggingArea.dragVelocity > 44) {
                if (panel.align === Qt.AlignBottom || panel.align === Qt.AlignRight) {
                    panel.state = "";
                } else {
                    panel.state = "spread";
                }
            } else {
                panel.state = (bar.v < bar.size / 2) ? "spread" : "";
            }
        }
    }

    Item {
        id: bar
        height: parent.height
        width: parent.width
        anchors {
            left: internal.orientation === Qt.Horizontal ? parent.left : undefined
            right: internal.orientation === Qt.Horizontal ? parent.right : undefined
            top: internal.orientation === Qt.Vertical ? parent.top : undefined
            bottom: internal.orientation === Qt.Vertical ? parent.bottom : undefined
        }

        property real size: internal.orientation === Qt.Horizontal ? height : width
        // v will always be in the range 0..size, where v==0 means spread, v==size means hidden.
        property real v: panel.active ? 0 : size

        y: panel.align === Qt.AlignTop ? -v : panel.align === Qt.AlignBottom ? v : 0
        x: panel.align === Qt.AlignLeft ? -v : panel.align === Qt.AlignRight ? v : 0
    }

}
