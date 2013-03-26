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

/*!
    \qmltype Icon
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The Icon component displays an icon from the icon theme.

    The icon theme contains a set of standard icons referred to by their name.
    Using icons whenever possible enhances consistency accross applications.
    Each icon has a name and can have different visual representations depending
    on the size requested.

    Icons can also be colorized. Setting the \l color property will make all pixels
    with the color #bebebe (defined by convention) colored.

    Example:
    \qml
    Icon {
        width: 64
        height: 64
        name: "search"
    }
    \endqml

    Example of colorization:
    \qml
    Icon {
        width: 64
        height: 64
        name: "search"
        color: "red"
    }
    \endqml

    Icon themes are created following the
    \l{http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html}{Freedesktop Icon Theme Specification}.
*/
Item {
    id: icon

    /*!
       The name of the icon to display.
       \qmlproperty string name
    */
    property string name

    /*!
       The color that all pixels that originally are #bebebe should take.
       \qmlproperty color color
    */
    property color color: Qt.rgba(0.0, 0.0, 0.0, 0.0)

    /*!
      \internal
      Whether or not a color has been set.
     */
    property bool __colorize: color != Qt.rgba(0.0, 0.0, 0.0, 0.0)

    Image {
        id: image

        /* Necessary so that icons are not loaded before a size is set. */
        property bool ready: false
        Component.onCompleted: ready = true

        anchors.fill: parent
        source: ready && width > 0 && height > 0 && icon.name ? "image://gicon/%1".arg(icon.name) : ""
        sourceSize {
            width: width
            height: height
        }
        cache: true
        visible: !icon.__colorize
    }

    ShaderEffect {
        id: colorizedImage

        anchors.fill: parent
        visible: icon.__colorize && image.status == Image.Ready

        property Image source: visible ? image : null
        property color keyColorOut: icon.color
        property color keyColorIn: "#bebebe"

        fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform sampler2D source;
            uniform highp vec4 keyColorOut;
            uniform highp vec4 keyColorIn;
            uniform lowp float qt_Opacity;
            void main() {
                lowp vec4 sourceColor = texture2D(source, qt_TexCoord0);
                gl_FragColor = mix(vec4(keyColorOut.rgb, 1.0) * sourceColor.a, sourceColor, step(0.1, distance(sourceColor.rgb / sourceColor.a, keyColorIn.rgb))) * qt_Opacity;
            }"
    }
}
