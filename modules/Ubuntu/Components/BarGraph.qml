/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 0.1 as Ubuntu

Item {
    id: graph
    
    property var model
    property real maximumValue
    property alias labels: labelsRepeater.model
    property alias backgroundColor: background.color
    property color color: Qt.rgba(0.4, 0.73, 0.4, 1.0)
    property real threshold: 16.0
    property color aboveThresholdColor: "#ff4e00"

    implicitWidth: units.gu(38)
    implicitHeight: units.gu(8)

    Rectangle {
        id: background
        anchors.fill: parent
        color: Qt.rgba(0.0, 0.0, 0.0, 0.8)
    }

    Ubuntu.TextureFromImage {
        id: texture
        image: graph.model.image
        Connections {
            target: graph.model
        }
    }

    ShaderEffect {
        anchors.fill: parent
        
        property var texture: texture
        property real shift: graph.model.shift / graph.model.samples
        property real maximumValue: graph.maximumValue
        property color color: graph.color
        property real threshold: graph.threshold
        property color aboveThresholdColor: graph.aboveThresholdColor

        vertexShader: "
                uniform highp mat4 qt_Matrix;
                attribute highp vec4 qt_Vertex;
                attribute highp vec2 qt_MultiTexCoord0;
                varying highp vec2 coord;
                void main() {
                    coord = qt_MultiTexCoord0;
                    gl_Position = qt_Matrix * qt_Vertex;
                }"

        fragmentShader: "
                varying highp vec2 coord;
                uniform sampler2D texture;
                uniform lowp float qt_Opacity;
                uniform lowp float shift;
                uniform lowp float maximumValue;
                uniform lowp vec4 color;
                uniform lowp float threshold;
                uniform lowp vec4 aboveThresholdColor;
                void main() {
                    lowp vec4 tex = texture2D(texture, vec2(coord.x + shift, coord.y));
                    lowp float value = tex.r * 255.0;
                    lowp float isOn = 1.0 - step(value / maximumValue, 1.0 - coord.y);
                    lowp float isAboveThreshold = step(threshold, value);

                    gl_FragColor = mix(vec4(0.0), mix(color, aboveThresholdColor, isAboveThreshold), isOn) * qt_Opacity;
                }"
    }

    Repeater {
        id: labelsRepeater

        Item {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                bottomMargin: modelData.value / graph.maximumValue * parent.height
            }

            Label {
                id: label
                anchors {
                    left: parent.left
                    leftMargin: units.gu(0.5)
                    verticalCenter: parent.verticalCenter
                }
                text: modelData.label
                color: modelData.color
                fontSize: "x-small"
            }

            Rectangle {
                anchors {
                    left: label.right
                    leftMargin: units.gu(0.5)
                    right: parent.right
                }
                height: 1
                color: modelData.color
            }
        }
    }
}
