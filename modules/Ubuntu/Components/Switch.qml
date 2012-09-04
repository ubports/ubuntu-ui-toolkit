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

// FIXME(loicm) The component copies code from Button.qml, though I guess it
//     could be reused quite easily. I'll need to figure that out.

import QtQuick 1.1
import Qt.labs.shaders 1.0

/*!
    \qmlclass Switch
    \inqmlmodule Ubuntu.Components 0.1
    \brief Switch is a component with two states, checked or unchecked. It can
    be used to set boolean options. The behavior is the same as CheckBox, the
    only difference is the graphical style.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        Switch {
            id: firstSwitch
            innerColor: "#097de0"
        }
    }
    \endqml
*/
Item {

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
       \preliminary
       The outer color of the Switch.
    */
    property color outerColor: "#c4c4c4"

    /*!
       \preliminary
       The inner color of the switch.
    */
    property color innerColor: "#8b8b8b"

    /*!
       \preliminary
       This handler is called when the switch is clicked.
    */
    signal clicked

    Keys.onEnterPressed: clicked()
    Keys.onReturnPressed: clicked()

    /*!
      \preliminary
      The mouse area of the check box. May be assigned a different mouse area
      if, for example, the area where the control is shown is not the same as
      the area where it accepts mouse events. This is used in list items with
      controls.
     */
    property MouseArea mouseArea: defaultMouseArea

    MouseArea {
        id: defaultMouseArea
        anchors.fill: parent
    }

    /*!
      \internal
      Connect the signals/slots of the new mouse area.
     */
    onMouseAreaChanged: internals.updateMouseArea()

    Item {
        anchors.fill: parent

        BorderImage {
            id: outerShape
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        Rectangle {
            id: outerBase
            anchors.fill: outerShape
            color: sweetch.outerColor
        }

        ButtonMaskEffect {
            anchors.fill: outerShape
            gradientStrength: 0.0
            mask: ShaderEffectSource { sourceItem: outerShape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: outerBase; live: true; hideSource: true }
        }

        BorderImage {
            id: outerBorder
            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.borderIdleSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14
        }

        // FIXME(loicm) The radius of the inner BorderImage must be lower than
        //     the radius of the outer BorderImage so that the perimeter can
        //     look perfectly consistent.

        BorderImage {
            id: innerShape
            x: outerShape.x + internals.spacing +
                (sweetch.checked ? ((outerShape.width - (2.0 * internals.spacing)) * 0.4) : 0.0)
            y: outerShape.y + internals.spacing
            width: (outerShape.width - (2.0 * internals.spacing)) * 0.6
            // FIXME(loicm) Note sure why one pixel needs to be removed in order
            //     to see the outer shape at the bottom. I'll figure that out
            //     while refactoring the code to correctly share common
            //     rendering code between the Button, CheckBox, Switch and
            //     ProgressBar components.
            height: outerShape.height - (2.0 * internals.spacing) - 1.0
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: internals.shapeSource
            border.left: 14; border.top: 14; border.right: 14; border.bottom: 14

            Behavior on x {
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }

        Rectangle {
            id: innerBase
            anchors.fill: innerShape
            color: sweetch.innerColor
        }

        ButtonMaskEffect {
            anchors.fill: innerShape
            gradientStrength: 0.0
            mask: ShaderEffectSource { sourceItem: innerShape; live: true; hideSource: true }
            base: ShaderEffectSource { sourceItem: innerBase; live: true; hideSource: true }
        }

        BorderImage {
            id: innerBorder
            anchors.fill: innerShape
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
        property url borderPressedSource: Qt.resolvedUrl("artwork/ButtonBorderPressed.png")
        property url borderIdleSource: Qt.resolvedUrl("artwork/ButtonBorderIdle.png")
        property real spacing: 3.0

        function updateMouseArea() {
            if (sweetch.mouseArea) {
                sweetch.mouseArea.clicked.connect(sweetch.clicked);
                sweetch.mouseArea.pressedChanged.connect(internals.mouseAreaPressed);
            }
        }

        function mouseAreaPressed() {
            if (sweetch.mouseArea.pressedButtons & Qt.LeftButton)
                sweetch.checked = !sweetch.checked;
        }
    }
}
