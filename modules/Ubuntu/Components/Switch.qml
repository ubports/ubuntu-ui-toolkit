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
    \qmlclass Switch
    \inqmlmodule Ubuntu.Components 0.1
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

    width: 87
    // FIXME(loicm) Button and other components have a default height of 39, I
    //     had to specify 42 here because under that value the BorderImage of
    //     the inner element gets cropped and starts to look ugly. A general
    //     solution would need to be found in order to support arbitrary sizes,
    //     or maybe simply clamping the requested sizes.
    height: 42

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

        BorderImage {
            id: backgroundShape
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Rectangle {
            id: backgroundBase
            anchors.fill: parent
            color: sweetch.checked ? internals.checkedColor : internals.uncheckedColor
        }

        ButtonMaskEffect {
            anchors.fill: parent
            gradientStrength: 0.0
            mask: ShaderEffectSource { sourceItem: backgroundShape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: backgroundBase; live: true; hideSource: true }
        }

        BorderImage {
            id: backgroundBorder
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Image {
            id: checkMark
            width: Math.min(parent.height - internals.iconSpacing,
                            (parent.width * 0.5) - internals.iconSpacing)
            height: checkMark.width
            x: internals.iconHorizontalMargin
            y: (parent.height - checkMark.height) * 0.5 - 1.0
            fillMode: Image.PreserveAspectFit
            source: internals.checkMarkSource
            opacity: sweetch.checked ? 1.0 : 0.0

            Behavior on opacity { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        Image {
            id: ballot
            width: Math.min(parent.height - internals.iconSpacing,
                            (parent.width * 0.5) - internals.iconSpacing)
            height: ballot.width
            x: parent.width - ballot.width - internals.iconHorizontalMargin
            y: (parent.height - ballot.height) * 0.5 - 1.0
            fillMode: Image.PreserveAspectFit
            source: internals.ballotSource
            opacity: sweetch.checked ? 0.0 : 1.0

            Behavior on opacity { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        // FIXME(loicm) The radius of the thumb BorderImage must be lower than
        //     the radius of the background BorderImage so that the perimeter can
        //     look perfectly consistent.
        // FIXME(loicm) There are stretched pixels on the left of the thumb in
        //     the checked state.

        BorderImage {
            id: thumbShape
            x: backgroundShape.x + internals.thumbSpacing +
               (sweetch.checked ? ((backgroundShape.width - (2.0 * internals.thumbSpacing))
               * (1.0 - internals.thumbWidth)) : 0.0)
            y: backgroundShape.y + internals.thumbSpacing
            width: (backgroundShape.width - (2.0 * internals.thumbSpacing)) * internals.thumbWidth
            height: backgroundShape.height - (2.0 * internals.thumbSpacing)
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14

            Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
        }

        Rectangle {
            id: thumbBase
            anchors.fill: thumbShape
            color: internals.thumbColor
        }

        ButtonMaskEffect {
            anchors.fill: thumbShape
            gradientStrength: 0.4
            mask: ShaderEffectSource { sourceItem: thumbShape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: thumbBase; live: true; hideSource: true }
        }

        BorderImage {
            id: thumbBorder
            anchors.fill: thumbShape
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
            visible: true
        }
    }

    QtObject {
        id: internals

        property url ballotSource: Qt.resolvedUrl("artwork/Ballot.png")
        property url checkMarkSource: Qt.resolvedUrl("artwork/CheckMark.png")
        property url shapeSource: Qt.resolvedUrl("artwork/ButtonShape.png")
        property url borderIdleSource: Qt.resolvedUrl("artwork/ButtonBorderIdle.png")
        property real iconHorizontalMargin: 8.0
        property real iconSpacing: 10.0
        property real thumbWidth: 0.5    // In [0.0, 1.0].
        property real thumbSpacing: 3.0
        property color uncheckedColor: "#c4c4c4"
        property color checkedColor: uncheckedColor
        property color thumbColor: "#8b8b8b"
    }
}
