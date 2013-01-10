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

// FIXME(loicm) It would be nice to have a draggable thumb to toggle the switch,
//     that needs to be validated with designers though.

import QtQuick 2.0

/*!
    \qmltype Switch
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Switch is a component with two states, checked or unchecked. It can
    be used to set boolean options. The behavior is the same as \l CheckBox, the
    only difference is the graphical style.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        Switch {
            checked: true
        }
    }
    \endqml
*/
AbstractButton {

    // FIXME(loicm) switch is a reserved keyword. Shouldn't we specify a prefix
    //     as a guideline to prevent that?
    id: sweetch

    width: units.gu(8)
    height: units.gu(4)

    /*!
      \preliminary
      Specifies whether the switch is checked or not. By default the property
      is set to false.
    */
    property bool checked: false

    /*!
      \internal
     */
    onClicked: sweetch.checked = !sweetch.checked

    Item {
        anchors.fill: parent

        opacity: enabled ? 1.0 : 0.5

        UbuntuShape {
            id: backgroundShape

            anchors.fill: parent
            color: sweetch.checked ? internals.checkedColor : internals.uncheckedColor
            gradientColor: "transparent"
        }

        Image {
            id: checkMark
            anchors {
                left: parent.left
                leftMargin: internals.iconHorizontalMargin
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -1.0
            }
            fillMode: Image.PreserveAspectFit
            source: internals.checkMarkSource
            opacity: sweetch.checked ? 1.0 : 0.0

            Behavior on opacity { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        Image {
            id: ballot
            anchors {
                right: parent.right
                rightMargin: internals.iconHorizontalMargin
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -1.0
            }
            fillMode: Image.PreserveAspectFit
            source: internals.ballotSource
            opacity: sweetch.checked ? 0.0 : 1.0

            Behavior on opacity { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        UbuntuShape {
            id: thumbShape

            x: backgroundShape.x + internals.thumbSpacing +
               (sweetch.checked ? ((backgroundShape.width - (2.0 * internals.thumbSpacing))
               * (1.0 - internals.thumbWidth)) : 0.0)
            y: backgroundShape.y + internals.thumbSpacing
            width: (backgroundShape.width - (2.0 * internals.thumbSpacing)) * internals.thumbWidth
            height: backgroundShape.height - (2.0 * internals.thumbSpacing)
            color: internals.thumbColor
            gradientColor: "transparent"

            Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }
    }

    QtObject {
        id: internals

        property url ballotSource: Qt.resolvedUrl("artwork/cross.png")
        property url checkMarkSource: Qt.resolvedUrl("artwork/ticker.png")
        property real iconHorizontalMargin: units.gu(1)
        property real iconSpacing: units.gu(1)
        property real thumbWidth: 0.5    // In [0.0, 1.0].
        property real thumbSpacing: units.dp(2)
        property color uncheckedColor: checkedColor
//        property color uncheckedColor: "#333333"
        property color checkedColor: "#dd4814"
        property color thumbColor: "#f1e2d1"
    }
}
