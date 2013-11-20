/*
 * Copyright 2013 Canonical Ltd.
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

Item {
    id: magnifier
    property alias sourceItem: effectSource.sourceItem
    property alias scaleFactor: effect.scaleFactor

    // everything in the sourceItem that is not transparent will be made this color
    // in the output, but the transparency of the input is respected
    property alias outputColor: effect.outputColor

    ShaderEffectSource {
        id: effectSource
        visible: false

        property real sourceRectMultiplier: 2.0

        // XXX: This works because the parent of magnifier is the same as sourceItem
        //  in this case. Otherwise coordinate transformations will be needed.
        sourceRect: Qt.rect(magnifier.x, magnifier.y, magnifier.width, magnifier.height)

        textureSize: Qt.size(magnifier.width*sourceRectMultiplier, magnifier.height*sourceRectMultiplier)
    }

    ShaderEffect {
        id: effect
        anchors.fill: parent

        property variant source: effectSource
        property real scaleFactor: 1.2
        property color outputColor: "red"

        vertexShader: "
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            uniform highp mat4 qt_Matrix;
            uniform highp float scaleFactor;
            varying highp vec2 qt_TexCoord0;
            void main() {
                qt_TexCoord0 = vec2(0.5 - 1.0 / (2.0 * scaleFactor)) + qt_MultiTexCoord0 / vec2(scaleFactor);
                gl_Position = qt_Matrix * qt_Vertex;
            }";

        fragmentShader: "
            uniform lowp float qt_Opacity;
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform highp vec4 outputColor;

            void main() {
                vec2 tc = qt_TexCoord0;
                lowp vec4 tex = texture2D(source, tc);
                gl_FragColor = vec4(outputColor.rgb, outputColor.a*tex.a) * qt_Opacity;
            }
            "
    }
}
