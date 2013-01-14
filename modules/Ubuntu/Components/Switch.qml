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

    //    width: units.gu(8)
    //    height: units.gu(4)
    width: backgroundShape.width
    height: backgroundShape.height

    opacity: enabled ? 1.0 : 0.5

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


    UbuntuShape {
        id: backgroundShape

        anchors.fill: parent
        color: Qt.rgba(0.86, 0.28, 0.08, 0.1) // ubuntu-orange with 0.1 opacity
        gradientColor: "transparent"

        height: thumbShape.height + 2*internals.thumbSpacing
        width: 2*thumbShape.width + 3*internals.thumbSpacing

        UbuntuShape {
            id: thumbShape

            width: units.gu(4)
            height: units.gu(4)

            y: backgroundShape.y + internals.thumbSpacing
            x: sweetch.checked ? rightItem.x : leftItem.x
            // FIXME: using uncheckedColor ignores the alpha component
            color: sweetch.checked ? internals.checkedColor : Qt.rgba(0.0, 0.0, 0.0, 0.1)
            gradientColor: "transparent"

            Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
            Behavior on color { ColorAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }


        Item {
            id: leftItem
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: internals.thumbSpacing
                topMargin: internals.thumbSpacing
            }
            height: thumbShape.height
            width: thumbShape.width

            Image {
                id: ballot
                anchors.centerIn: parent
                source: internals.ballotSource
            }
        }

        Item {
            id: rightItem
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: internals.thumbSpacing
                topMargin: internals.thumbSpacing
            }
            height: thumbShape.height
            width: thumbShape.width

            Image {
                id: checkMark
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: internals.checkMarkSource
            }
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
        //        property color uncheckedColor: checkedColor
        //        property color uncheckedColor: "#333333"
        property color checkedColor: "#DD4814"
        property color uncheckedColor: Qt.rgba(1.0, 1.0, 0.0, 0.5) //Qt.rgba(0.0, 0.0, 0.0, 0.1)
        //        property color thumbColor: "#f1e2d1"
    }
}
