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

import QtQuick 1.1
import Qt.labs.shaders 1.0

/*!
    \qmlclass Button
    \inqmlmodule Ubuntu.Components 0.1
    \brief The Button class is DOCME

    \b{This component is under heavy development.}

    Examples:
    \qml
        Column {
            Button {
                text: "Send"
                onClicked: print("clicked text-only Button")
            }
            Button {
                iconSource: "icon.png"
                color: "green"
                onClicked: print("clicked icon-only Button")
            }
            Button {
                iconSource: "icon.png"
                text: "Icon on left"
                iconPosition: "left"
                onClicked: print("clicked text and icon Button")
            }
        }
    \endqml
*/
ButtonWithForeground {
    id: button

    width: 87
    height: 39

    /*!
       \preliminary
       The foreground color of the button in idle state.
    */
    property color color: style.color//"#e3e5e8"

    /*!
       \preliminary
       The color when the button is pressed.
    */
    property color pressedColor: color

    /*!
       \preliminary
       Specify whether the layer behind the button has a dark color.
       This will change the visuals/shading of the button.
    */
    property bool darkBorder: false

    // pick either a clear or dark text color depending on the luminance of the
    // background color to maintain good contrast (works in most cases)
    /*!
      \internal
     */
    textColor: __luminance(base.color) <= 0.72 ? "white" : "#757373"

    /*!
      \internal
     */
    function __luminance(hexcolor){
        hexcolor = String(hexcolor)
        var r = parseInt(hexcolor.substr(1,2),16);
        var g = parseInt(hexcolor.substr(3,2),16);
        var b = parseInt(hexcolor.substr(5,2),16);
        return ((r*212)+(g*715)+(b*73))/1000/255;
    }
    //theming
    onPressedChanged: {
        state = pressed ? 'pressed' : ''
        console.debug("button pressed= "+pressed+", state= "+state)
    }

    Item {
        z: -1
        anchors.fill: parent

        // FIXME: think of using distance fields
        BorderImage {
            id: shape

            anchors.fill: parent

            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: style.borderShape//button.darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
            border.left: 18; border.top: 15
            border.right: 18; border.bottom: 15
        }

        // FIXME: might become a paper texture
        Rectangle {
            id: base

            anchors.fill: shape
            color: button.pressed ? button.pressedColor : button.color

        }

        ButtonMaskEffect {
            anchors.fill: shape
            gradientStrength: button.pressed ? 0.0 : 1.0
            Behavior on gradientStrength {NumberAnimation {duration: 100; easing.type: Easing.OutQuad}}

            mask: ShaderEffectSource {sourceItem: shape; live: true; hideSource: true}
            base: ShaderEffectSource {sourceItem: base; live: true; hideSource: true}
        }

        // FIXME: could be generated from the shape (shadow parameters specified in guidelines)
        BorderImage {
            id: border

            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: style.borderImage
            //source: if (button.darkBorder) return button.pressed ? "artwork/ButtonBorderDarkPressed.png" : "artwork/ButtonBorderDarkIdle.png"
            //        else return button.pressed ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png"
            border.left: 14; border.top: 17
            border.right: 15; border.bottom: 18
        }
    }
}
