/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.0
/*!
    \qmltype Button
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief Standard Ubuntu button.

    \l {http://design.ubuntu.com/apps/building-blocks/buttons}{See also the Design Guidelines on Buttons}.

    Examples:
    \qml
        Column {
            Button {
                text: "Send"
                onClicked: print("clicked text-only Button")
            }
            Button {
                iconName: "compose"
                gradient: UbuntuColors.greyGradient
                onClicked: print("clicked icon-only Button")
            }
            Button {
                iconName: "compose"
                text: "Icon on left"
                iconPosition: "left"
                onClicked: print("clicked text and icon Button")
            }
        }
    \endqml
    An \l Action can be used to specify \b clicked, iconSource and text. Example:
    \qml
        Item {
            Action {
                id: action1
                text: "Click me"
                onTriggered: print("action!")
                iconName: "compose"
            }
            Button {
                anchors.centerIn: parent
                action: action1
                color: UbuntuColors.warmGrey
            }
       }
    \endqml
*/

/*
   Documentation in Button11.qdoc
*/
Button {
    /*!
      \internal
     */
    property color strokeColor: Qt.rgba(0.0, 0.0, 0.0, 0.0)
}
