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

//    anchors {
//        left: internal.orientation === Qt.Horizontal || panel.align === Qt.AlignLeft ? parent.left : undefined
//        right: internal.orientation === Qt.Horizontal || panel.align === Qt.AlignRight ? parent.right : undefined
//        top: internal.orientation === Qt.Vertical || panel.align === Qt.AlignTop ? parent.top : undefined
//        bottom: internal.orientation === Qt.Vertical || panel.align === Qt.AlignBottom ? parent.bottom : undefined
//    }

//      anchors.fill: parent
//    height: units.gu(10)

//    Rectangle {
//        color: "red"
//        opacity: 0.5
//        anchors.fill: bar
//    }

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
        The default value is \b Qt.AlignBottom.
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
      How much of the toolbar to show when starting interaction.
     */
    property real hintSize: units.gu(2)

    /*!
      The height of the mouse area used to detect edge swipes to
      activate the toolbar.
     */
    property real triggerSize: units.gu(2)

    states: [
        State {
            name: "hint"
            PropertyChanges {
                target: bar
                w: bar.size - panel.hintSize
            }
        },
        State {
            name: "moving"
            PropertyChanges {
                target: bar
                w: MathUtils.clamp(draggingArea.mouseW - internal.movingDelta, 0, bar.size)
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: bar
                w: 0
            }
        },
        State {
            name: ""
            PropertyChanges {
                target: bar
                w: bar.size
                explicit: true
            }
        }
    ]

    /*!
      The toolbar is currently not in a stable hidden or visible state.
     */
    readonly property bool animating: draggingArea.pressed || (state == "" && bar.w != bar.size) || (state == "spread" && bar.w != 0)

    transitions: [
        Transition {
            to: ""
            PropertyAnimation {
                target: bar
                properties: "w"
                duration: 500 // TODO TIM: revert to 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "hint"
            PropertyAnimation {
                target: bar
                properties: "w"
                duration: 50 // TODO TIM: revert to 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "spread"
            PropertyAnimation {
                target: bar
                properties: "w"
                duration: 500 // TODO TIM: revert to 50
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

        readonly property int orientation: (panel.align === Qt.AlignTop || panel.align === Qt.AlignBottom) ? Qt.Horizontal : Qt.Vertical
    }

    Connections {
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
                // if the toolbar was locked, do not slide it back in
                // until the user performs a bottom-edge-swipe.
            }
        }
    }

    onStateChanged: {
        if (state == "hint") {
            internal.movingDelta = panel.hintSize + draggingArea.initialW - bar.size;
        } else if (state == "moving" && internal.previousState == "spread") {
            internal.movingDelta = draggingArea.initialW;
        } else if (state == "spread") {
            panel.active = true;
        } else if (state == "") {
            panel.active = false;
        }
        internal.previousState = state;
    }

    // not working for now.
    // TODO: link to bug report or uncomment
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
        orientation: Qt.Vertical
        id: draggingArea
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: panel.active ? bar.height + units.gu(1) : panel.triggerSize
        zeroVelocityCounts: true
        propagateComposedEvents: true
        visible: !panel.lock

        // FIXME: Weird construction below, but it doesn't seem to work
        //  in onPressed when using mouseW directly.
        property int mouseW: getMouseW()
        function getMouseW() {
            return internal.orientation === Qt.Horizontal ? mouseY : mouseX
        }

        property int initialW
        onPressed: {
            initialW = getMouseW();
//            mouse.accepted = false
//            if (panel.state == "spread") mouse.accepted = false;
//            mouse.accepted = false;
            if (panel.state == "") panel.state = "hint";
//            else mouse.accepted = false;
//            else panel.state = "moving";
        }

        onPositionChanged: {
            if (panel.state == "hint" && mouseW < initialW) {
                panel.state = "moving";
            } else if (panel.state == "spread" && mouseW > initialW) {
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
                panel.state = "spread";
            } else if (draggingArea.dragVelocity > 44) {
                panel.state = "";
            } else {
                panel.state = (bar.w < bar.size / 2) ? "spread" : "";
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
//        height: internal.orientation === Qt.Horizontal ? size : undefined
//        width: internal.orientation === Qt.Vertical ? size : undefined
//        y: panel.active ? 0 : height
        property real w: panel.active ? 0 : size
        y: internal.orientation === Qt.Horizontal ? w : 0
        x: internal.orientation === Qt.Vertical ? w : 0
    }

}
