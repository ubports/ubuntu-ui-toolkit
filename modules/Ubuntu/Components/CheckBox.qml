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

/*!
    \qmltype CheckBox
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief CheckBox is a component with two states, checked or unchecked. It can
    be used to set boolean options. The behavior is the same as \l Switch, the
    only difference is the graphical style.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        CheckBox {
            checked: true
        }
    }
    \endqml
*/
AbstractButton {
    id: checkBox

//    width: units.gu(4)
//    height: units.gu(4)
    width: background.width
    height: background.height

    /*!
      \preliminary
      Specifies whether the checkbox is checked or not. By default the property
      is set to false.
    */
    property bool checked: false

    /*!
      \internal
     */
    onClicked: checked = !checked

    Item {
        z: -1
        anchors.fill: parent

        opacity: enabled ? 1.0 : 0.5

        Image {
            id: background
            source: internals.thumbSource
        }

        Image {
            id: checkMark

            anchors.centerIn: parent
            smooth: true
            source: internals.tickerSource

            opacity: checkBox.checked ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }

        Image {
            id: uncheckedMark

            anchors.centerIn: parent
            smooth: true
            source: internals.crossSource

            opacity: checkBox.checked ? 0.0 : 1.0
            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }
    }

    QtObject {
        id: internals

        property url thumbSource: Qt.resolvedUrl("artwork/checkbox/thumb.png")
        property url tickerSource: Qt.resolvedUrl("artwork/checkbox/ticker.png")
        property url crossSource: Qt.resolvedUrl("artwork/checkbox/cross.png")
        property color uncheckedColor: checkedColor
        property color checkedColor: "#626262"
    }
}
