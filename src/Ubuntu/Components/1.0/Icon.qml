/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4

Item {
    id: icon

    property string name

    /*!
       \qmlproperty color color
    */
    property alias color: colorizedImage.keyColorOut

    /*!
       \qmlproperty color keyColor
    */
    property alias keyColor: colorizedImage.keyColorIn

    implicitWidth: image.implicitWidth
    implicitHeight: image.implicitHeight

    Image {
        id: image
        objectName: "image"
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit

        sourceSize {
            width: icon.width
            height: icon.height
        }

        source: {
            if (icon.hasOwnProperty("source"))
                return icon.source;
            else if (icon.name)
                return "image://theme/%1".arg(icon.name);
            else
                return "";
        }

        cache: true
        visible: !colorizedImage.visible
    }

    ShaderEffect {
        id: colorizedImage
        objectName: "shader"

        anchors.fill: parent

        // Whether or not a color has been set.
        visible: image.status == Image.Ready && keyColorOut != Qt.rgba(0.0, 0.0, 0.0, 0.0)

        property Image source: image
        property color keyColorOut: Qt.rgba(0.0, 0.0, 0.0, 0.0)
        property color keyColorIn: "#808080"
        property real threshold: 0.1

        fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform highp vec4 keyColorOut;
            uniform highp vec4 keyColorIn;
            uniform lowp float threshold;
            uniform lowp float qt_Opacity;
            void main() {
                lowp vec4 sourceColor = texture2D(source, qt_TexCoord0);
                gl_FragColor = mix(vec4(keyColorOut.rgb, 1.0) * sourceColor.a, sourceColor, step(threshold, distance(sourceColor.rgb / sourceColor.a, keyColorIn.rgb))) * qt_Opacity;
            }"
    }
}
