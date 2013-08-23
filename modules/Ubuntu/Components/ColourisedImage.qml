/*
 * Copyright (C) 2013 Canonical, Ltd.
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

import QtQuick 2.0

Item {
    id: colourisedImage

    /*!
       Image source.
       \qmlproperty url source
    */
    property url source

    /*!
       The color that all pixels that originally are of color \l keyColor should take.
       \qmlproperty color color
    */
    property color colour

    /*!
       The color of the pixels that should be colorized.
       \qmlproperty color keyColor
    */
    property color keyColour

    /*!
       Whether or not our colour shader is currently active.
       \qmlproperty bool active
    */
    property bool active: true

    /*!
       Our image's sourceSize
       \qmlproperty size sourceSize
    */
    property alias sourceSize: image.sourceSize

    Image {
        id: image

        anchors.fill: parent
        source: source
        cache: true
        visible: !colourShader.active
    }

    ShaderEffect {
        id: colourShader

        anchors.fill: parent
        visible: active && image.status == Image.Ready

        property Image source: visible ? image : null
        property color keyColourOut: colour
        property color keyColourIn: keyColour
        property real threshold: 0.1

        fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform highp vec4 keyColourOut;
            uniform highp vec4 keyColourIn;
            uniform lowp float threshold;
            uniform lowp float qt_Opacity;
            void main() {
                lowp vec4 sourceColour = texture2D(source, qt_TexCoord0);
                gl_FragColor = mix(vec4(keyColourOut.rgb, 1.0) * sourceColour.a, sourceColour, step(threshold, distance(sourceColour.rgb / sourceColour.a, keyColourIn.rgb))) * qt_Opacity;
            }"
    }
}
