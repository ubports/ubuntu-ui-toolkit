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
*/
Item {
    id: panel

    anchors {
        left: internal.orientation === Qt.Horizontal || panel.align === Qt.AlignLeft ? parent.left : undefined
        right: internal.orientation === Qt.Horizontal || panel.align === Qt.AlignRight ? parent.right : undefined
        top: internal.orientation === Qt.Vertical || panel.align === Qt.AlignTop ? parent.top : undefined
        bottom: internal.orientation === Qt.Vertical || panel.align === Qt.AlignBottom ? parent.bottom : undefined
    }

    default property alias contents: bar.data

    /*!
      The property defines the alignment of the panel.
      The implementation supports the following values:
        \list
        \li Qt.AlignLeft anchors to the left
        \li Qt.AlignRight anchors to the right
        \li Qt.AlignTop anchors to the top
        \li Qt.AlignBottom anchors to the bottom
        \endlist
        The default value is \b Qt.AlignBottom.

        By default, the panel will anchor to the specified side and fill the whole edge, but the
        anchors of the panel may be overridden.
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
    property real hintSize: units.gu(1)

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
                y: bar.height - panel.hintSize
            }
        },
        State {
            name: "moving"
            PropertyChanges {
                target: bar
                y: MathUtils.clamp(draggingArea.mouseY - internal.movingDelta, 0, bar.height)
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
                explicit: true
            }
        }
    ]

    /*!
      The toolbar is currently not in a stable hidden or visible state.
     */
    readonly property bool animating: draggingArea.pressed || (state == "" && bar.y != bar.height) || (state == "spread" && bar.y != 0)

    transitions: [
        Transition {
            to: ""
            PropertyAnimation {
                target: bar
                properties: "y"
                duration: 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "hint"
            PropertyAnimation {
                target: bar
                properties: "y"
                duration: 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "spread"
            PropertyAnimation {
                target: bar
                properties: "y"
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
            internal.movingDelta = panel.hintSize + draggingArea.initialY - bar.height;
        } else if (state == "moving" && internal.previousState == "spread") {
            internal.movingDelta = draggingArea.initialY;
        } else if (state == "spread") {
            panel.active = true;
        } else if (state == "") {
            panel.active = false;
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

        y: panel.active ? 0 : height
    }

    Toolkit.InverseMouseArea {
        anchors.fill: draggingArea
        onClicked: {
            mouse.accepted = false;
            // the mouse click may cause an update
            //  of lock by the clicked Item behind
            if (!panel.lock) panel.active = false;
        }
        propagateComposedEvents: true
        visible: panel.lock == false && panel.state == "spread"
    }

    DraggingArea {
        orientation: Qt.Vertical
        id: draggingArea
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: panel.active ? bar.height + units.gu(1) : toolbar.triggerSize
        zeroVelocityCounts: true
        propagateComposedEvents: true
        visible: !panel.lock

        property int initialY
        onPressed: {
            initialY = mouseY;
            if (panel.state == "") panel.state = "hint";
            else panel.state = "moving";
        }

        onPositionChanged: {
            if (panel.state == "hint" && mouseY < initialY) {
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
                panel.state = (bar.y < bar.height / 2) ? "spread" : "";
            }
        }
    }
}
