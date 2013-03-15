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

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.Empty {
                height: units.gu(6)
                Text {
                    text: "Hello world"
                    anchors.centerIn: parent
                }
            }
            ListItem.Empty {
                height: units.gu(13)
                Tabs {
                    height: parent.height - units.gu(3)
                    width: parent.width - units.gu(3)
                    anchors.centerIn: parent
                    Tab {
                        text: "Tab 1"
                        page: Label {
                            anchors.centerIn: parent
                            text: "Page one"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 2"
                        page: Label {
                            anchors.centerIn: parent
                            text: "Page two"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 3"
                        page: Label {
                            anchors.centerIn: parent
                            text: "Page three"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
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

    width: parent ? parent.width : units.gu(31)
    height: __height + bottomDividerLine.height

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
      The property holds the attached action to the list item.
      */
    property Action action

    Rectangle {
        id: highlight
        visible: emptyListItem.selected || (emptyListItem.highlightWhenPressed && emptyListItem.pressed)
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: __height
        color: "#E6E6E6"
        opacity: 0.7
    }

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
    property bool showDivider: __showDivider()

    /*!
      \internal
      Method to automatically determine if the bottom divider line should be drawn.
      This always returns true, unless item is a delegate in a ListView. If in a ListView
      it will return false only when:
       + if there is a section.delegate below this item (as thin divider line and section
         delegate should not both appear)
       + if this is the final item in the list, and ListView.footer is set (again as thin
         divider line won't look well with footer below it)
     */
    // FIXME: The new design shows dividers everywhere, so if it does not change anymore,
    //  the __showDivider() function may be removed.
    function __showDivider() {
        // if we're not in ListView, always show a thin dividing line at the bottom
        //if (ListView.view !== null) {

            // if we're last item in ListView, show divider if no footer is defined
            // and hide it if footer defined
            //if (index === ListView.view.model.count - 1) return !ListView.footer;

            // if section.delegate is positioned between this item and the next
            //else if (ListView.section !== ListView.nextSection) return true;
            //else return false;
        //}
        return true;
    }

    ThinDivider {
        id: bottomDividerLine
        anchors.bottom: parent.bottom
        visible: showDivider && (__height > 0)
    }

    /*!
      \internal
      Reparent so that the visuals of the children does not
      occlude the bottom divider line.
     */
    default property alias children: body.children

    /*!
      \preliminary
      Defines the background color of the rectangle used in the background during the item removal
     */
    property alias backgroundColor: __backgroundItem.color

    /*!
      \preliminary
      Defines the text color of the rectangle used in the background during the item removal
     */
    property alias backgroundTextColor: __backgroundText.color

    /*!
      \preliminary
      Defines the display text visible during the item removal
     */
    property alias backgroundTex: __backgroundText.text

    Item {
        id: bodyMargins

        clip: true
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: bottomDividerLine.top
            margins: emptyListItem.__contentsMargins
        }

        Item {
            id: body

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: parent.width

            Behavior on x {
                SequentialAnimation {
                    NumberAnimation {
                        duration: 200
                    }
                    ScriptAction {
                         script: {
                             emptyListItem.commitDrag()
                        }
                    }
                }
            }

            onXChanged: {
                if (x > 0) {
                    __backgroundItem.state = "SLIDING-RIGHT"
                } else {
                    __backgroundItem.state = "SLIDING-LEFT"
                }
            }
        }

        Rectangle {
            id: __backgroundItem

            color: "gray"
            opacity: 0.0

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            Label {
                id: __backgroundText

                property int slidingMargin: units.gu(3)

                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "Clear"
                fontSize: "medium"
                color: "#e8e1d0"
                font.bold: true
            }

            states: [
                State {
                    name: "SLIDING-RIGHT"
                    AnchorChanges {
                        target: __backgroundItem
                        anchors.left: parent.left
                        anchors.right: body.left
                    }
                    PropertyChanges {
                        target: __backgroundText
                        anchors.rightMargin: slidingMargin
                        anchors.leftMargin: 0
                        horizontalAlignment: Text.AlignRight

                    }
                    PropertyChanges {
                        target: __backgroundItem
                        opacity: 1.0
                    }
                },
                State {
                    name: "SLIDING-LEFT"
                    AnchorChanges {
                        target: __backgroundItem
                        anchors.left: body.right
                        anchors.right: parent.right
                    }
                    PropertyChanges {
                        target: __backgroundText
                        anchors.rightMargin: 0
                        anchors.leftMargin: slidingMargin
                        horizontalAlignment: Text.AlignLeft
                    }
                    PropertyChanges {
                        target: __backgroundItem
                        opacity: 1.0
                    }
                }
            ]
        }
    }

    /*! \internal
      The spacing inside the list item.
     */
    property real __contentsMargins: units.gu(0)

    /*!
      \preliminary
      Defines if this item can be removed or not.
     */
    property bool removable: false

    /*!
      \preliminary
      This is a read-only property that notify when the item is swiping
     */
    property bool swiping: false

    /*! \internal
      Defines the offset used when the item will start to move
     */
    property int __mouseMoveOffset: units.gu(4)

    /*! \internal
      Defines the offset limit to consider the item removed
     */
    property int __itemMoveOffset: width * 0.3

    /*! \internal
      Defines the inital pressed possition
     */
    property int __pressedPosition: 0

    /*! \internal
      Defines the final pressed possition
     */
    property int __positionEnded: 0

    /*! \internal
      Defines if the item is moving or not
     */
    property bool __held: false

    /*! \internal
      Defines if the item should be removed after the animation or not
     */
    property bool __removeItem: false

    /*!
      \preliminary
      This handler is called when the item is removed from the list
     */
    signal itemRemoved

    __mouseArea.drag.axis: Drag.XAxis

    SequentialAnimation {
        id: __removeItemAnimation
        NumberAnimation {
            target: emptyListItem
            property: "__height"
            to: 0
            duration: 200
        }
        ScriptAction {
             script: itemRemoved()
        }
    }

    Connections {
        target: __mouseArea

        onPressed: {
            if (!emptyListItem.removable) {
                return;
            }

            emptyListItem.__pressedPosition = mouse.x
        }

        onMouseXChanged: {
            if (!emptyListItem.removable) {
                return;
            }

            var mouseOffset = emptyListItem.__pressedPosition - mouse.x
            if ((emptyListItem.__pressedPosition != -1) && !emptyListItem.__held && ( Math.abs(mouseOffset) >= emptyListItem.__mouseMoveOffset)) {
                emptyListItem.startDrag();
            }
        }

        onPositionChanged: {
            if (!emptyListItem.removable) {
                return;
            }

            if (emptyListItem.__held) {
                emptyListItem.__positionEnded = body.x;
            }
        }

        onReleased: {
            if (!emptyListItem.removable) {
                return;
            }

            if (emptyListItem.__held) {
                emptyListItem.endDrag();
            }
        }
    }

    // notify the start of the drag operation
    function startDrag() {
        swiping = true
        body.anchors.left = undefined
        __mouseArea.drag.target = body

        __held = true
        __mouseArea.drag.maximumX = parent.width
        __mouseArea.drag.minimumX = (width * -1)
        __backgroundItem.visible = true
    }

    // Commit the necessary changes to remove or not the item based on the mouse final position
    function commitDrag() {
        __pressedPosition = -1
        __mouseArea.drag.target = null
        __mouseArea.drag.target = null
        __held = false
        __positionEnded = body.x

        if (__removeItem) {
            __removeItemAnimation.start()
        }
        __removeItem = false

        swiping = false
        __backgroundItem.state = ""
    }

    // notify the releaso of the mouse button and the end of the drag operation
    function endDrag() {
        if (Math.abs(body.x) < __itemMoveOffset && __held == true) {
            __removeItem = false
            if (body.x == 0) {
                commitDrag()
            } else {
                body.x = 0;
            }
        } else if (__held == true) {
            __removeItem = true
            if (body.x > 0) {
                body.x = body.width
            } else {
                body.x = body.width * -1
            }
        }
    }
}
