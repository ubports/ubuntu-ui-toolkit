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
import Ubuntu.Components 1.2

Item {
    id: root
    width: 1000
    height: 600
    focus: true

    // Enum to string tables.
    property variant aspectTable: [
        "Flat", "Inset"
    ]
    property variant backgroundModeTable: [
        "SolidColor", "VerticalGradient"
    ]
    property variant sourceHAlignmentTable: [
        "AlignLeft", "AlignHCenter", "AlignRight"
    ]
    property variant sourceVAlignmentTable: [
        "AlignTop", "AlignVCenter", "AlignBottom"
    ]
    property variant sourceFillModeTable: [
        "Stretch", "PreserveAspectFit", "PreserveAspectCrop", "Pad"
    ]
    property variant wrapModeTable: [
        "Transparent", "Repeat"
    ]
    property variant imageFillModeTable: [
        "Stretch", "PreserveAspectFit", "PreserveAspectCrop", "Tile", "TileVertically",
        "TileHorizontally", "Pad"
    ]

    // Zoom properties.
    property variant translation: Qt.point(0.0, 0.0)
    property real scaleBase: 1.1
    property real scaleExponent: 0.0
    property real minScaleExponent: 0.0
    property real maxScaleExponent:  48.317715856  // Logarithm of 100 to base 1.1 (scaleBase).
    property real scaleFactor: 1.0
    property real scale: 1.0

    // Overlay text properties. French keymapping... :)
    property string textOverlayString:
        "Zoom            (scroll):   x " + root.scaleFactor.toFixed(1) + "\n\n" +
        "Background colors  (a/z):   " + shape.backgroundColor + ", " + shape.secondaryBackgroundColor + "\n" +
        "Background mode      (e):   " + root.backgroundModeTable[shape.backgroundMode] + "\n\n" +
        "Radius               (r):   " + "\"" + shape.radius + "\"\n" +
        "Aspect               (t):   " + root.aspectTable[shape.aspect] + "\n\n" +
        "Source               (o):   " + shape.source + "\n" +
        "Source opacity       (p):   " + shape.sourceOpacity.toFixed(2) + "\n" +
        "Source fill          (q):   " + root.sourceFillModeTable[shape.sourceFillMode] + "\n" +
        "Source hwrap         (s):   " + root.wrapModeTable[shape.sourceHorizontalWrapMode] + "\n" +
        "Source vwrap         (d):   " + root.wrapModeTable[shape.sourceVerticalWrapMode] + "\n" +
        "Source halign        (f):   " + root.sourceHAlignmentTable[shape.sourceHorizontalAlignment] + "\n" +
        "Source valign        (g):   " + root.sourceVAlignmentTable[shape.sourceVerticalAlignment] + "\n" +
        "Source translation (h/j):   " + shape.sourceTranslation.x.toFixed(2) + ", " + shape.sourceTranslation.y.toFixed(2) + "\n" +
        "Source scale       (k/l):   " + shape.sourceScale.x.toFixed(2) + ", " + shape.sourceScale.y.toFixed(2) + "\n\n" +
        "Image (deprecated)   (m):   " + shape.image + "\n" +
        "Image fill           (w):   " + root.imageFillModeTable[img1.fillMode] + "\n" +
        "Image halign         (x):   " + img1.horizontalAlignment + "\n" +
        "Image valign         (c):   " + img1.verticalAlignment + "\n\n" +
        "Border (deprecated)  (b):   " + "\"" + shape.borderSource + "\"\n\n" +
        "Colors (deprecated) (n/,):  " + shape.color + ", " + shape.gradientColor +  "\n\n" +
        "Overlay color       (F1):   " + shape.overlayColor + "\n" +
        "Overlay rect (F2/F3/F4/F5): " + shape.overlayRect.x.toFixed(2) + ", " + shape.overlayRect.y.toFixed(2) + ", " + shape.overlayRect.width.toFixed(2) + ", " + shape.overlayRect.height.toFixed(2)

    // Main scene.
    Item {
        id: scene
        anchors.fill: parent

        Image {
            id: background
            anchors.fill: parent
            source: "background.jpg"
            fillMode: Image.Tile
        }

        // Put the UbuntuShape source image in the middle of a texture atlas. We use img1.
        Image { id: img1; visible: false; source: "img1.png"; }
        Image { id: img2; visible: false; source: "img2.png"; }
        Image { id: img3; visible: false; source: "img3.png"; }
        Image { id: img4; visible: false; source: "img4.png"; }

        UbuntuShapeOverlay {
            id: shape
            anchors.fill: parent
            anchors.leftMargin: 400
            anchors.rightMargin: 100
            anchors.topMargin: 100
            anchors.bottomMargin: 100
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
        var alt = Qt.AltModifier;

        // Background.
        if (event.key == Qt.Key_A) {
            shape.backgroundColor = Qt.rgba(
                Math.random(), Math.random(), Math.random(), Math.random());
        } else if (event.key == Qt.Key_Z) {
            shape.secondaryBackgroundColor = Qt.rgba(
                Math.random(), Math.random(), Math.random(), Math.random());
        } else if (event.key == Qt.Key_E) {
            shape.backgroundMode = (shape.backgroundMode + 1) % 3;

        // Styling.
        } else if (event.key == Qt.Key_R) {
            shape.radius = (shape.radius == "large") ? "small" :
                ((shape.radius == "small") ? "medium" : "large");
        } else if (event.key == Qt.Key_T) {
            shape.aspect = (shape.aspect + 1) % 2;

        // Source.
        } else if (event.key == Qt.Key_O) {
            if (shape.source == null) {
                shape.source = img1;
            } else {
                shape.source = null;
            }
        } else if (event.key == Qt.Key_P) {
            shape.sourceOpacity = Math.max(0.0, Math.min(
                1.0, shape.sourceOpacity + ((event.modifiers & shift) ? 0.01 : -0.01)));
        } else if (event.key == Qt.Key_Q) {
            shape.sourceFillMode = (shape.sourceFillMode + 1) % 4;
        } else if (event.key == Qt.Key_S) {
            shape.sourceHorizontalWrapMode = (shape.sourceHorizontalWrapMode + 1) % 3;
        } else if (event.key == Qt.Key_D) {
            shape.sourceVerticalWrapMode = (shape.sourceVerticalWrapMode + 1) % 3;
        } else if (event.key == Qt.Key_F) {
            shape.sourceHorizontalAlignment = (shape.sourceHorizontalAlignment + 1) % 3;
        } else if (event.key == Qt.Key_G) {
            shape.sourceVerticalAlignment = (shape.sourceVerticalAlignment + 1) % 3;
        } else if (event.key == Qt.Key_H) {
            shape.sourceTranslation = Qt.vector2d(
                shape.sourceTranslation.x + ((event.modifiers & shift) ? 0.01 : -0.01),
                shape.sourceTranslation.y);
        } else if (event.key == Qt.Key_J) {
            shape.sourceTranslation = Qt.vector2d(
                shape.sourceTranslation.x,
                shape.sourceTranslation.y + ((event.modifiers & shift) ? 0.01 : -0.01));
        } else if (event.key == Qt.Key_K) {
            shape.sourceScale = Qt.vector2d(
                shape.sourceScale.x + ((event.modifiers & shift) ? 0.02 : -0.02),
                shape.sourceScale.y);
        } else if (event.key == Qt.Key_L) {
            shape.sourceScale = Qt.vector2d(
                shape.sourceScale.x,
                shape.sourceScale.y + ((event.modifiers & shift) ? 0.02 : -0.02));

        // Deprecated image.
        } else if (event.key == Qt.Key_M) {
            if (shape.image == null) {
                shape.image = img1;
            } else {
                shape.image = null;
            }
        } else if (event.key == Qt.Key_W) {
            img1.fillMode = (img1.fillMode + 1) % 7;
        } else if (event.key == Qt.Key_X) {
            if (img1.horizontalAlignment == Image.AlignLeft) {
                img1.horizontalAlignment = Image.AlignHCenter;
            } else if (img1.horizontalAlignment == Image.AlignHCenter) {
                img1.horizontalAlignment = Image.AlignRight;
            } else {
                img1.horizontalAlignment = Image.AlignLeft;
            }
        } else if (event.key == Qt.Key_C) {
            if (img1.verticalAlignment == Image.AlignTop) {
                img1.verticalAlignment = Image.AlignVCenter;
            } else if (img1.verticalAlignment == Image.AlignVCenter) {
                img1.verticalAlignment = Image.AlignBottom;
            } else {
                img1.verticalAlignment = Image.AlignTop;
            }

        // Deprecated styling.
        } else if (event.key == Qt.Key_B) {
            if (shape.borderSource == "radius_idle.sci") {
                shape.borderSource = "radius_pressed.sci";
            } else if (shape.borderSource == "radius_pressed.sci") {
                shape.borderSource = "";
            } else {
                shape.borderSource = "radius_idle.sci";
            }
        } else if (event.key == Qt.Key_N) {
            shape.color = Qt.rgba(
                Math.random(), Math.random(), Math.random(), Math.random());
        } else if (event.key == Qt.Key_Comma) {
            shape.gradientColor = Qt.rgba(
                Math.random(), Math.random(), Math.random(), Math.random());

        // Overlay.
        } else if (event.key == Qt.Key_F1) {
            shape.overlayColor = Qt.rgba(
                Math.random(), Math.random(), Math.random(), Math.random());
        } else if (event.key == Qt.Key_F2) {
            var x = Math.max(0.0, Math.min(1.0,
            shape.overlayRect.x + ((event.modifiers & shift) ? 0.005 : -0.005)));
            shape.overlayRect = Qt.rect(
                x, shape.overlayRect.y, shape.overlayRect.width,
                shape.overlayRect.height);
        } else if (event.key == Qt.Key_F3) {
            var y = Math.max(0.0, Math.min(1.0,
            shape.overlayRect.y + ((event.modifiers & shift) ? 0.005 : -0.005)));
            shape.overlayRect = Qt.rect(
                shape.overlayRect.x, y, shape.overlayRect.width,
                shape.overlayRect.height);
        } else if (event.key == Qt.Key_F4) {
            var width = Math.max(0.0, Math.min(1.0,
            shape.overlayRect.width + ((event.modifiers & shift) ? 0.005 : -0.005)));
            shape.overlayRect = Qt.rect(
                shape.overlayRect.x, shape.overlayRect.y, width,
                shape.overlayRect.height);
        } else if (event.key == Qt.Key_F5) {
            var height = Math.max(0.0, Math.min(1.0,
            shape.overlayRect.height + ((event.modifiers & shift) ? 0.005 : -0.005)));
            shape.overlayRect = Qt.rect(
                shape.overlayRect.x, shape.overlayRect.y, shape.overlayRect.width,
                height);
        }
    }
}
