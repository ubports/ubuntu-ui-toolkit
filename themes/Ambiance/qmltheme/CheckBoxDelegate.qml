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
    implicitWidth: border.sourceSize.width
    implicitHeight: border.sourceSize.height

    Item {
        anchors.fill: parent

        opacity: enabled ? 1.0 : 0.5

        ShaderEffect {
            id: background
            anchors.fill: parent

            property ShaderEffectSource mask: ShaderEffectSource {
                sourceItem: BorderImage {
                    id: mask
                    source: itemStyle.maskSource
                    width: border.width
                    height: border.height
                }
            }

            property color color: checked ? itemStyle.checkedColor : itemStyle.uncheckedColor

            // FIXME: animate script doesn't work here?
            Behavior on color { ScriptAction { script: StyleUtils.animate("backgroundColorAnimation") } }

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
            source: itemStyle.thumbSource
        }

        Image {
            id: checkMark

            anchors.centerIn: parent
            smooth: true
            source: itemStyle.tickerSource

            visible: checkBox.checked
        }

        Image {
            id: uncheckedMark

            anchors.centerIn: parent
            smooth: true
            source: itemStyle.crossSource

            visible: !checkBox.checked
        }
    }
}
