/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3

Item {
    id: root
    width: 500
    height: width * (15.0 / 16.0)
    focus: true

    // Zoom properties.
    property variant translation: Qt.point(0.0, 0.0)
    property real scaleBase: 1.1
    property real scaleExponent: 0.0
    property real minScaleExponent: 0.0
    property real maxScaleExponent:  48.317715856  // Logarithm of 100 to base 1.1 (scaleBase).
    property real scaleFactor: 1.0
    property real scale: 1.0

    // Overlay text properties.
    property string textOverlayString:
        "Zoom  (scroll):   x " + root.scaleFactor.toFixed(1) + "\n" +
        "Width      (a):   " + shape.width.toFixed(2) + "\n" +
        "Height     (z):   " + shape.height.toFixed(2)

    // Main scene.
    Item {
        id: scene
        anchors.fill: parent

        Rectangle {
            id: background
            anchors.fill: parent
            color: "white"
        }

        ProportionalShape {
            id: shape
            anchors.centerIn: parent
            width: 300
            source: Image { source: "music-app.png" }
        }
    }

    // Zoom support.
    ShaderEffectSource {
        id: shaderEffectSource
        anchors.fill: scene
        sourceItem: scene
        hideSource: true
        visible: false
        smooth: false
    }
    ShaderEffect {
        anchors.fill: scene
        property variant tex: shaderEffectSource
        property variant translation: root.translation
        property real scaleFactor: root.scale
        vertexShader: "
            uniform mat4 qt_Matrix;
            uniform float scaleFactor;
            uniform vec2 translation;
            attribute vec4 qt_Vertex;
            attribute vec2 qt_MultiTexCoord0;
            varying vec2 texCoord;
            void main() {
                texCoord = vec2(scaleFactor) * qt_MultiTexCoord0 + translation;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: "
            uniform sampler2D tex;
            uniform float qt_Opacity;
            varying vec2 texCoord;
            void main() {
                gl_FragColor = texture2D(tex, texCoord) * qt_Opacity;
            }"
    }

    // Text overlay.
    Text {
        id: textOverlay
        width:200
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Ubuntu Mono"
        font.pixelSize: 14
        font.weight: Font.Bold
        color: "black"
        text: textOverlayString
    }

    // Mouse handling.
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true

        property real lastX: 0.0
        property real lastY: 0.0

        onPressed: {
            if (pressedButtons & Qt.LeftButton) {
                lastX = mouseX;
                lastY = mouseY;
            }
        }
        onPositionChanged: {
            if (pressedButtons & Qt.LeftButton) {
                var tx = root.translation.x;
                var ty = root.translation.y;
                var sx = root.scale / root.width;
                var sy = root.scale / root.height;
                var x = mouseX - lastX;
                var y = mouseY - lastY;
                root.translation = Qt.point(Math.max(0.0, Math.min(1.0 - root.scale, tx - sx * x)),
                                            Math.max(0.0, Math.min(1.0 - root.scale, ty - sy * y)));
                lastX = mouseX;
                lastY = mouseY;
            }
        }
        onWheel: {
            root.scaleExponent = Math.max(minScaleExponent, Math.min(maxScaleExponent,
                root.scaleExponent + (wheel.angleDelta.y < 0.0 ? -1.0 : 1.0)));
            root.scaleFactor = Math.pow(root.scaleBase, root.scaleExponent);
            var oldScale = root.scale;
            root.scale = 1.0 / root.scaleFactor;
            var s = oldScale - root.scale;
            var tx = root.translation.x;
            var ty = root.translation.y;
            var x = mouseX / root.width;
            var y = mouseY / root.height;
            root.translation = Qt.point(Math.max(0.0, Math.min(1.0 - root.scale, tx + s * x)),
                                        Math.max(0.0, Math.min(1.0 - root.scale, ty + s * y)));
        }
    }

    // Keyboard handling.
    Keys.onPressed: {
        var shift = Qt.ShiftModifier;

        if (event.key == Qt.Key_A) {
            shape.width = Math.max(0.0, shape.width + ((event.modifiers & shift) ? 2.0 : -2.0));
        } else if (event.key == Qt.Key_Z) {
            shape.height = Math.max(0.0, shape.height + ((event.modifiers & shift) ? 2.0 : -2.0));
        }
    }
}
