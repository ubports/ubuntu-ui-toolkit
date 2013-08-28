/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 0.1

/*!
    \qmltype Empty
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief A list item with no contents.
    The Empty class can be used for generic list items, containing other
    components such as buttons. It is selectable, and can take mouse clicks.
    It will attempt to detect if a thin dividing line at the bottom of the
    item is suitable, but this behaviour can be over-ridden (using \l showDivider).
    For specific types of list items, see its subclasses.

    After the item removal the item will still alive is up to the application to destroy it,
    this can be handled by the signal \l itemRemoved that is fired after all animation is done.

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        Item {
            Model {
                id: contactModel

                ListElement {
                    name: "Bill Smith"
                    number: "555 3264"
                }
                ListElement {
                    name: "John Brown"
                    number: "555 8426"
                }
            }

            ListView {
                 width: 180; height: 200
                 model: contactModel

                 delegate: ListItem.Empty {
                    height: units.gu(6)
                    removable: true
                    onItemRemoved: contactModel.remove(index)
                    Text {
                        text: name + " " + number
                        anchors.centerIn: parent
                    }
                }
            }
        }
    \endqml

    See the documentation of the derived classes of Empty for more examples.
    \b{This component is under heavy development.}
*/
AbstractButton {
    id: emptyListItem

    /*!
      \preliminary
      Specifies whether the list item is selected.
     */
    property bool selected: false

    /*!
      \preliminary
      Highlight the list item when it is pressed.
      This is used to disable the highlighting of the full list item
      when custom highlighting needs to be implemented (for example in
      ListItem.Standard which can have a split).
    */
    property bool highlightWhenPressed: true

    /*!
      \preliminary
      Defines if this item can be removed or not.
     */
    property bool removable: false

    /*!
      \preliminary
      \qmlproperty string swipingState
      The current swiping state ("SwipingLeft", "SwipingRight", "")
     */
    readonly property alias swipingState: backgroundIndicator.state


    /*!
      \preliminary
      This handler is called when the item is removed from the list
     */
    signal itemRemoved

    /*!
      \internal
      Defines the height of the ListItem, so correct height of this component, including divider
      line is calculated.
     */
    property int __height: units.gu(6)

    /*!
      \preliminary
      Set to show or hide the thin bottom divider line (drawn by the \l ThinDivider component).
      This line is shown by default except in cases where this item is the delegate of a ListView.
     */
    property bool showDivider: true

    /*!
      \internal
      Reparent so that the visuals of the children does not
      occlude the bottom divider line.
     */
    default property alias children: body.children

     /*!
      \internal
      Allows derived class to proper add items inside of this element
      */
    property alias __contents: body

    /*!
      \preliminary
      \qmlproperty list<Item> backgroundIndicator
      Defines the item background item to be showed during the item swiping
     */
    property alias backgroundIndicator: backgroundIndicator.children

    /*!
      \preliminary
      \qmlproperty ThinDivider bottomDividerLine
      Exposes our the bottom line divider.
     */
    property alias divider: bottomDividerLine

    /*! \internal
      The spacing inside the list item.
     */
    property real __contentsMargins: units.gu(2)

    width: parent ? parent.width : units.gu(31)
    implicitHeight: priv.removed ? 0 : __height + bottomDividerLine.height
    __mouseArea.drag.axis: Drag.XAxis

    // Keep compatible with the old version
    height: implicitHeight

    /*! \internal */
    QtObject {
        id: priv

        /*! \internal
          Defines the offset used when the item will start to move
         */
        readonly property int mouseMoveOffset: units.gu(1)

        /*! \internal
          Defines the offset limit to consider the item removed
         */
        readonly property int itemMoveOffset: width * 0.3

        /*! \internal
          Defines the inital pressed possition
         */
        property int pressedPosition: -1

        /*! \internal
          Defines if the item is moving or not
         */
        property bool held: false

        /*! \internal
          Defines if the item should be removed after the animation or not
         */
        property bool removeItem: false

        /*! \internal
          Defines if the item was removed or not
         */
        property bool removed: false

        /*! \internal
            notify the start of the drag operation
         */
        function startDrag() {
            __mouseArea.drag.target = body
            held = true
            __mouseArea.drag.maximumX = parent.width
            __mouseArea.drag.minimumX = (parent.width * -1)
            backgroundIndicator.visible = true
        }

        /*! \internal
            Resets the item dragging state
         */
        function resetDrag() {
            pressedPosition = -1
            __mouseArea.drag.target = null
            held = false
            removeItem = false
            backgroundIndicator.state = ""
        }

        /*! \internal
           Commit the necessary changes to remove or not the item based on the mouse final position
        */
        function commitDrag() {
            if (removeItem) {
                removeItemAnimation.start()
            } else {
                resetDrag()
            }
        }

        /*! \internal
            notify the releaso of the mouse button and the end of the drag operation
        */
        function endDrag() {
            if (Math.abs(body.x) < itemMoveOffset && held == true) {
                held = false
                removeItem = false
                if (body.x == 0) {
                    resetDrag()
                } else {
                    body.x = 0;
                }
            } else if (held == true) {
                held = false
                removeItem = true
                if (body.x > 0) {
                    body.x = body.width
                } else {
                    body.x = body.width * -1
                }
            }
        }
    }

    ThinDivider {
        id: bottomDividerLine
        anchors.bottom: parent.bottom
        visible: showDivider && !priv.removed
    }

    Item {
        id: bodyMargins

        clip: body.x != 0
        visible: body.height > 0
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        Item {
            id: body

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: parent.width

            Behavior on x {
                enabled: !priv.held
                SequentialAnimation {
                    UbuntuNumberAnimation {
                    }
                    ScriptAction {
                         script: {
                             priv.commitDrag()
                        }
                    }
                }
            }

            onXChanged: {
                if (x > 0) {
                    backgroundIndicator.state = "SwipingRight"
                } else {
                    backgroundIndicator.state = "SwipingLeft"
                }
            }
        }

        Item {
            id: backgroundIndicator

            opacity: 0.0
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            states: [
                State {
                    name: "SwipingRight"
                    AnchorChanges {
                        target: backgroundIndicator
                        anchors.left: parent.left
                        anchors.right: body.left
                    }
                    PropertyChanges {
                        target: backgroundIndicator
                        opacity: 1.0
                    }
                },
                State {
                    name: "SwipingLeft"
                    AnchorChanges {
                        target: backgroundIndicator
                        anchors.left: body.right
                        anchors.right: parent.right
                    }
                    PropertyChanges {
                        target: backgroundIndicator
                        opacity: 1.0
                    }
                }
            ]
        }
    }

    SequentialAnimation {
        id: removeItemAnimation

        running: false
        UbuntuNumberAnimation {
            target: emptyListItem
            property: "implicitHeight"
            to: 0
        }
        ScriptAction {
             script: {
                 priv.removed = true
                 itemRemoved()
                 priv.resetDrag()
             }
        }
    }

    Connections {
        target: (emptyListItem.removable) ? __mouseArea : null

        onPressed: {
            priv.pressedPosition = mouse.x
        }

        onMouseXChanged: {
            var mouseOffset = priv.pressedPosition - mouse.x
            if ((priv.pressedPosition != -1) && !priv.held && ( Math.abs(mouseOffset) >= priv.mouseMoveOffset)) {
                priv.startDrag();
            }
        }

        onReleased: {
            priv.endDrag();
        }

        onCanceled: {

            priv.endDrag();
        }
    }
}
