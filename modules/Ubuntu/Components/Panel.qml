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
    \brief A panel that can be swiped in and out from an edge of the window by the user.
    For most applications, it is highly recommended to use the \l MainView instead which includes
    a toolbar at its bottom that can be swiped in or out.

    Unless your application has very specific needs for a Panel, use a \l MainView with the
    default toolbar.

    When using a Panel, do not theme it directly, but theme its contents, because
    the Panel itself should not have visible elements, since it is
    in the view (to detect mouse events) even when its contents should be invisible.

    Set the anchors and/or width/height of the Panel to specify the area that the Panel covers when
    activated. A panel that looks like the standard toolbar, but with custom contents,
    can be created like this:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        Rectangle {
            color: "grey"
            width: units.gu(80)
            height: units.gu(80)

            Panel {
                id: panel
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: units.gu(8)

                Item {
                    anchors.fill: parent
                    ItemStyle.class: "toolbar"

                    // two properties used by the toolbar delegate:
                    property bool active: panel.active
                    property bool animating: panel.animating

                    Button {
                        anchors.centerIn: parent
                        text: "Click"
                    }
                }
            }
        }
    \endqml

    Similarly, a black panel that can be swiped in from the lower-right of the window, and
    that covers the right-half of the window when activated can be created like this:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        Rectangle {
            color: "grey"
            width: units.gu(80)
            height: units.gu(80)

            Panel {
                id: panel
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    top: parent.top
                }
                width: parent.width / 2

                Rectangle {
                    anchors.fill: parent
                    color: "black"

                    Button {
                        anchors.centerIn: parent
                        text: "Click"
                    }
                }
            }
        }
    \endqml

    Any Items can be placed inside the Panel, but MouseAreas can block mouse events from reaching
    the panel and thus obstruct the swiping behavior for hiding the panel. As a result, the user cannot
    start swiping on the buttons in the examples above in order to hide the panel. To remedy this, clicked()
    signals are forwarded from the panel to its children. Example:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        Rectangle {
            color: "grey"
            width: units.gu(40)
            height: units.gu(40)

            Panel {
                id: panel
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: units.gu(8)

                Rectangle {
                    color: "white"
                    anchors.fill: parent
                    Rectangle {
                        width: units.gu(8)
                        height: units.gu(4)
                        anchors.centerIn: parent
                        color: "red"
                        signal clicked()
                        onClicked: print("The red rectangle was clicked");
                    }
                }
            }
        }
    \endqml
    Like this, the red rectangle accepts clicked() events, but the user can still swipe down on top
    of the rectangle in order to hide the panel.
*/
Item {
    id: panel

    /*!
      Default property of Panel is overridden to put the children inside
      the Item that will be swiped in and out
     */
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
      When active, the panel is visible, otherwise it is hidden.
      Use edge swipes to activate/deactivate the panel.
      The active property is not updated until the swipe gesture is completed.
     */
    property bool active: false
    /*! \internal */
    onActiveChanged: {
        if (active) state = "spread";
        else state = "";
    }

    /*!
      Disable edge swipe to activate/deactivate the panel.
     */
    property bool lock: false
    /*! \internal */
    onLockChanged: {
        if (state == "hint" || state == "moving") {
            draggingArea.finishMoving();
        }
    }

    /*!
      How much of the panel to show when the user touches the panel's edge.
      This gives the user a hint that there is a panel hiding at that edge and
      invites him/her to swipe to show the panel completely.
     */
    property real hintSize: units.gu(2)

    /*!
      The size (height for top or bottom-aligned panels, width for left or right-aligned
      panels) of the mouse area used to detect edge swipes to activate the panel, when
      the panel is not active.
     */
    property real triggerSize: units.gu(2)

    states: [
        State {
            name: "hint"
            PropertyChanges {
                target: bar
                position: bar.size - panel.hintSize
            }
        },
        State {
            name: "moving"
            PropertyChanges {
                target: bar
                position: MathUtils.clamp(draggingArea.mousePosition - internal.movingDelta, 0, bar.size)
            }
        },
        State {
            name: "spread"
            PropertyChanges {
                target: bar
                position: 0
            }
        },
        State {
            name: ""
            PropertyChanges {
                target: bar
                position: bar.size
                explicit: true
            }
        }
    ]

    /*!
      The toolbar is currently not in a stable hidden or visible state.
     */
    readonly property bool animating: draggingArea.pressed || (state == "" && bar.position != bar.size) || (state == "spread" && bar.position != 0)

    transitions: [
        Transition {
            to: ""
            PropertyAnimation {
                target: bar
                properties: "position"
                duration: 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "hint"
            PropertyAnimation {
                target: bar
                properties: "position"
                duration: 50
                easing.type: Easing.OutQuad
            }
        },
        Transition {
            to: "spread"
            PropertyAnimation {
                target: bar
                properties: "position"
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
        //  bottomBarVisibilityCommunicator forced the toolbar to hide.
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

    /*! \internal */
    onStateChanged: {
        if (state == "hint") {
            internal.movingDelta = panel.hintSize + draggingArea.initialPosition - bar.size;
        } else if (state == "moving" && internal.previousState == "spread") {
            internal.movingDelta = draggingArea.initialPosition;
        } else if (state == "spread") {
            panel.active = true;
        } else if (state == "") {
            panel.active = false;
        }
        internal.previousState = state;
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
        id: draggingArea
        orientation: internal.orientation === Qt.Horizontal ? Qt.Vertical : Qt.Horizontal
        zeroVelocityCounts: true
        anchors {
            top: panel.align === Qt.AlignBottom ? undefined : parent.top
            bottom: panel.align === Qt.AlignTop ? undefined : parent.bottom
            left: panel.align === Qt.AlignRight ? undefined : parent.left
            right: panel.align === Qt.AlignLeft ? undefined : parent.right
        }
        height: internal.orientation === Qt.Horizontal ? panel.active ? bar.size + units.gu(1) : panel.triggerSize : undefined
        width: internal.orientation === Qt.Vertical ? panel.active ? bar.size + units.gu(1) : panel.triggerSize : undefined
        visible: !panel.lock || panel.active

        property int mousePosition: getMousePosition()
        function getMousePosition() {
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

        // set in onPressed, reset when entering "moving" state
        property Item pressedItem: null

        // find the first child with a clicked property:
        function getClickableItem(mouse) {
            var item = bar; // contains the children
            while (item && !item.hasOwnProperty("clicked")) {
                var coords = mapToItem(item, mouse.x, mouse.y);
                item = item.childAt(coords.x, coords.y);
            }
            return item; // will be null if no item has clicked() signal.
        }

        // forward clicked events to any child Item with a clicked() signal, not
        // just MouseAreas since MouseAreas would block swiping of the panel.
        // This must also happen when the panel is locked, so the DraggingArea is
        // never disabled, and other signal handlers will return when panel.lock is true.
        onClicked: {
            if (pressedItem && pressedItem === getClickableItem(mouse)) {
                // Click event positioned at the Item where the user first pressed
                pressedItem.clicked();
            }
        }

        property int initialPosition
        onPressed: {
            pressedItem = getClickableItem(mouse)
            if (panel.lock) return;
            initialPosition = getMousePosition();
            if (panel.state == "") panel.state = "hint";
        }

        onPositionChanged: {
            if (panel.lock) return;
            if (panel.state == "hint" && mousePosition < initialPosition) {
                panel.state = "moving";
                pressedItem = null;
            } else if (panel.state == "spread" && mousePosition > initialPosition) {
                panel.state = "moving";
                pressedItem = null;
            }
        }

        onReleased: {
            if (panel.lock) return;
            finishMoving();
        }
        // Mouse cursor moving out of the window while pressed on desktop
        onCanceled: {
            if (panel.lock) return;
            finishMoving();
        }

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
                panel.state = (bar.position < bar.size / 2) ? "spread" : "";
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
        //position will always be in the range 0..size, where position==0 means spread, position==size means hidden.
        property real position: panel.active ? 0 : size

        y: panel.align === Qt.AlignTop ? -position : panel.align === Qt.AlignBottom ? position : 0
        x: panel.align === Qt.AlignLeft ? -position : panel.align === Qt.AlignRight ? position : 0
    }
}
