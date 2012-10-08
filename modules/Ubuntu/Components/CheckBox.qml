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
    \qmlclass CheckBox
    \inqmlmodule Ubuntu.Components 0.1
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

    width: units.dp(49)
    height: units.dp(39)

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

        // FIXME: think of using distance fields
        BorderImage {
            id: shape

            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        // FIXME: might become a paper texture
        Rectangle {
            id: base

            anchors.fill: shape

            color: checkBox.checked ? internals.checkedColor : internals.uncheckedColor
            Behavior on color {
                ColorAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }

        ButtonMaskEffect {
            anchors.fill: shape
            gradientStrength: 0.0
            mask: ShaderEffectSource { sourceItem: shape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: base; live: true; hideSource: true }
        }

        // FIXME: could be generated from the shape (shadow parameters specified in guidelines)
        BorderImage {
            id: border

            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: checkBox.checked ? internals.borderPressedSource : internals.borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Image {
            id: checkMark

            anchors.fill: parent
            anchors.margins: units.dp(5)
            fillMode: Image.PreserveAspectFit
            smooth: true
            source: internals.checkMarkSource

            opacity: checkBox.checked ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }
    }

    QtObject {
        id: internals

        property url checkMarkSource: Qt.resolvedUrl("artwork/CheckMark.png")
        property url shapeSource: Qt.resolvedUrl("artwork/ButtonShape.png")
        property url borderPressedSource: Qt.resolvedUrl("artwork/ButtonBorderPressed.png")
        property url borderIdleSource: Qt.resolvedUrl("artwork/ButtonBorderIdle.png")
        property color uncheckedColor: "#e3e5e8"
        property color checkedColor: "#8b8b8b"
    }
}
