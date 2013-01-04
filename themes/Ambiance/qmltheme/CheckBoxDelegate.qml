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
import Ubuntu.Components 0.1

Item {
    anchors.fill: parent

    Item {
//        z: -1
        anchors.fill: parent
//        width: border.width
//        height: border.height

        opacity: enabled ? 1.0 : 0.5

        ShaderEffect {
            id: background
            anchors.fill: parent

            property ShaderEffectSource mask: ShaderEffectSource {
                sourceItem: BorderImage {
                    id: mask
                    source: internals.maskSource
                    width: border.width
                    height: border.height
                }
            }

            property color color: checked ? internals.checkedColor : internals.uncheckedColor

            // TODO: Enable when CheckBox is themable
            Behavior on color { //ScriptAction { script: StyleUtils.animate("backgroundColorAnimation") } }
                ColorAnimation {
                    duration: 100
                }
            }

            fragmentShader:
                "
                varying highp vec2 qt_TexCoord0;
                uniform lowp float qt_Opacity;
                uniform sampler2D mask;
                uniform lowp vec4 color;

                void main(void) {
                    lowp vec4 maskColor = texture2D(mask, qt_TexCoord0.st);
                    if (maskColor.a == 0.0) discard;
                    gl_FragColor = color * vec4(maskColor.a * qt_Opacity);
                }
                "
        }

        BorderImage {
            id: border
            anchors.fill: parent
            source: internals.thumbSource
        }

        Image {
            id: checkMark

            anchors.centerIn: parent
            smooth: true
            source: internals.tickerSource

            visible: checkBox.checked
        }

        Image {
            id: uncheckedMark

            anchors.centerIn: parent
            smooth: true
            source: internals.crossSource

            visible: !checkBox.checked
        }
    }

    QtObject {
        id: internals

        property url maskSource: Qt.resolvedUrl("artwork/checkbox/thumb_shape.sci")
        property url thumbSource: Qt.resolvedUrl("artwork/checkbox/thumb.sci")
        property url tickerSource: Qt.resolvedUrl("artwork/checkbox/ticker.png")
        property url crossSource: Qt.resolvedUrl("artwork/checkbox/cross.png")
        property color uncheckedColor: "#724242" //checkedColor
        property color checkedColor: "#427242" //#626262"

        property PropertyAnimation backgroundColorAnimation: ColorAnimation { duration: 1000 }
    }

}
