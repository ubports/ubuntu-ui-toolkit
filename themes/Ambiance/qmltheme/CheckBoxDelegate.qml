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
                    source: StyleUtils.itemStyleProperty("maskSource", Qt.resolvedUrl("artwork/checkbox/mask.sci"))
                    width: border.width
                    height: border.height
                }
            }

            property color color: item.checked ? StyleUtils.itemStyleProperty("checkedColor") : StyleUtils.itemStyleProperty("uncheckedColor")

            // FIXME: animate script doesn't work with Behavior?
            Behavior on color { ScriptAction { script: StyleUtils.animate("backgroundColorAnimation") } }

            fragmentShader:
                "
                varying highp vec2 qt_TexCoord0;
                uniform lowp float qt_Opacity;
                uniform sampler2D mask;
                uniform lowp vec4 color;

                void main(void) {
                    lowp vec4 maskColor = texture2D(mask, qt_TexCoord0.st);
                    gl_FragColor = color * vec4(maskColor.a * qt_Opacity);
                }
                "
        }

        BorderImage {
            id: border
            anchors.fill: parent
            source: StyleUtils.itemStyleProperty("borderSource", Qt.resolvedUrl("artwork/checkbox/border.sci"))
        }

        Image {
            id: checkMark
            anchors.centerIn: parent
            smooth: true
            source: StyleUtils.itemStyleProperty("tickerSource")
            visible: item.checked
        }
    }
}
