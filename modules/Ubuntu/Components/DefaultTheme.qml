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
import Ubuntu.Components 0.1
import Qt.labs.shaders 1.0

Item{

    Style {
        // this style component defines the default visuals of a Button control
        selector: ".Button"
        visuals: Component {
            Item {
                z: -1
                anchors.fill: control

                // FIXME: think of using distance fields
                BorderImage {
                    id: shape

                    anchors.fill: parent

                    horizontalTileMode: BorderImage.Stretch
                    verticalTileMode: BorderImage.Stretch
                    source: styleItem.borderShape
                    border.left: 18; border.top: 15
                    border.right: 18; border.bottom: 15
                }

                // FIXME: might become a paper texture
                Rectangle {
                    id: base

                    anchors.fill: shape
                    color: control.pressed ? control.pressedColor : control.color

                }

                ButtonMaskEffect {
                    anchors.fill: shape
                    gradientStrength: control.pressed ? 0.0 : 1.0
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
                    source: styleItem.borderImage
                    border.left: 14; border.top: 17
                    border.right: 15; border.bottom: 18
                }
            }
        }
    }

    Style {
        // this style component defines the custom properties applied on the component
        selector: ".button"
        style: ButtonStyle {
            color: control.pressed ? "purple" : control.hovered ? "cyan" : "pink"//"#e3e5e8"
            borderShape: control.darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
            borderImage: (control.darkBorder) ? (control.pressed ? "artwork/ButtonBorderDarkPressed.png" : "artwork/ButtonBorderDarkIdle.png")
                    : (control.pressed ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png")
        }
    }
}
