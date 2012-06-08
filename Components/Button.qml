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
    \inqmlmodule UbuntuUIToolkit
    \brief The Button class is DOCME

    \b{This component is under heavy development.}

    Examples:
    \qml
        Button {
            text: "Send"
            onClicked: print("clicked text-only Button")
        }

        Button {
            iconSource: "call_icon.png"
            color: "green"
            onClicked: print("clicked icon-only Button")
        }

        Button {
            iconSource: "call_icon.png"
            text: "Icon on left"
            iconPosition: "left"
            onClicked: print("clicked text and icon Button")
        }
    \endqml
*/
ButtonWithForeground {
    id: button

    width: 87
    height: 39

    /*!
       \preliminary
       DOCME
    */
    property color color: "#e3e5e8"

    /*!
       \preliminary
       DOCME
    */
    property color pressedColor: color

    // pick either a clear or dark text color depending on the luminance of the
    // background color to maintain good contrast (works in most cases)
    textColor: luminance(base.color) <= 0.8 ? "white" : "#757373"

    function luminance(hexcolor){
        hexcolor = String(hexcolor)
        var r = parseInt(hexcolor.substr(1,2),16);
        var g = parseInt(hexcolor.substr(3,2),16);
        var b = parseInt(hexcolor.substr(5,2),16);
        return ((r*299)+(g*587)+(b*114))/1000/255;
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
            source: "artwork/ButtonShape.png"
            border.left: 18; border.top: 15
            border.right: 18; border.bottom: 15
        }

        // FIXME: might become a paper texture
        Rectangle {
            id: base

            anchors.fill: shape
            color: button.state != "pressed" ? button.color : button.pressedColor
        }

        // Composed with Overlay mode
        Rectangle {
            id: gradient

            anchors.fill: shape
            opacity: 0.8
            gradient: Gradient {
                GradientStop { position: 0.0; color: "white" }
                GradientStop { position: 1.0; color: "black" }
            }
        }

        ButtonMaskEffect {
            anchors.fill: shape
            gradientStrength: button.state != "pressed" ? 1.0 : 0.0
            Behavior on gradientStrength {NumberAnimation {duration: 100; easing.type: Easing.OutQuad}}

            mask: ShaderEffectSource {sourceItem: shape; live: false; hideSource: true}
            base: ShaderEffectSource {sourceItem: base; live: true; hideSource: true}
            gradient: ShaderEffectSource {sourceItem: gradient; live: false; hideSource: true}
        }

        // FIXME: could be generated from the shape (shadow parameters specified in guidelines)
        BorderImage {
            id: border

            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: button.state == "pressed" ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png"
            border.left: 18; border.top: 19
            border.right: 18; border.bottom: 19
        }
    }
}
