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
    ShaderEffect {
        id: effect
        anchors.fill: parent

        property color color: "white"

        // FIXME: It would be nicer to have a single transformation matrix that flips and rotates,
        //  but I did not manage to get a 3x3 matrix from QML into the shader.

        // rotate pointer 90 degrees
        property bool rotate: (item.direction === "left" || item.direction === "right")

        // flip the direction of the pointer
        property bool flip: (item.direction === "left" || item.direction === "up")

        visible: (item.direction !== "none")

        vertexShader: "
            uniform highp mat4 qt_Matrix;
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;

            uniform bool flip;
            uniform bool rotate;
            varying highp vec2 coord;

            void main() {
                if (rotate) coord = qt_MultiTexCoord0.ts;
                else coord = qt_MultiTexCoord0.st;

                if (flip) coord = vec2(coord.x, 1.0 - coord.y);

                gl_Position = qt_Matrix * qt_Vertex;
            }
        "

        fragmentShader: "
            varying highp vec2 coord;
            uniform vec4 color;
            uniform float opacity;

            void main(void) {
                if (coord.t > 2.0*(1.0 - coord.s)) discard;
                if (coord.t > 2.0*coord.s) discard;
                gl_FragColor = color * vec4(opacity);
            }
        "
    }
}
