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
import Ubuntu.Components.ListItems 0.1

/*!
    \qmlclass SingleControl
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief A list item containing a single control

    Examples:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.SingleControl {
                control: Button {
                    anchors {
                        margins: 5
                        fill: parent
                    }
                    text: "Large button"
                }
            }
        }
    \endqml

    \b{This component is under heavy development.}
*/
// TODO: Add more examples when more types of controls become available.
Empty {
    id: singleControlListItem

    height: control ? control.height + control.anchors.topMargin + control.anchors.bottomMargin : undefined

    /*!
      \preliminary
      The control of this SingleControl list item.
      The control will automatically be re-parented to, and centered in, this list item.
     */
    property Item control

    onClicked: control.clicked(mouse)
    pressed: __mouseArea.pressed || control.__mouseArea.pressed
    onPressedChanged: control.pressed = singleControlListItem.pressed

    /*!
      \internal
     */
    function __updateControl() {
        if (control) {
            control.parent = singleControlListItem;
            control.anchors.centerIn = singleControlListItem;
//            control.mouseArea = singleControlListItem.mouseArea;
        }
    }

    /*!
      \internal
      This handler is an implementation detail. Mark as internal to prevent QDoc publishing it
     */
    onControlChanged: __updateControl()
//    Component.onCompleted: __updateControl()
}
