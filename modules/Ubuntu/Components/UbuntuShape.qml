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
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype UbuntuShape
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The UbuntuShape item provides a standard Ubuntu shaped rounded rectangle.

    The UbuntuShape is used where a rounded rectangle is needed either filled
    with a color or an image that it crops.

    When given with a \l color it is applied with an overlay blending as a
    vertical gradient going from \l color to \l gradientColor.
    Two corner \l radius are available, "small" (default) and "medium", that
    determine the size of the corners.
    Optionally, an Image can be passed that will be displayed inside the
    UbuntuShape and cropped to fit it.

    Examples:
    \qml
        import Ubuntu.Components 0.1

        UbuntuShape {
            color: "lightblue"
            radius: "medium"
        }
    \endqml

    \qml
        import Ubuntu.Components 0.1

        UbuntuShape {
            image: Image {
                source: "icon.png"
            }
        }
    \endqml

    \qml
        import Ubuntu.Components 0.1

        UbuntuShape {
            maskSource: "customMask.sci"
            borderSource: "customBorder.sci"
        }
    \endqml

*/
Item {
    id: shape

    Theming.ItemStyle.class: "UbuntuShape-radius-" + radius

    /*!
      The size of the corners among: "small" (default) and "medium".
    */
    property string radius: "small"

    /*!
      The top color of the gradient used to fill the shape. Setting only this
      one is enough to set the overall color the shape.
    */
    property color color: Qt.rgba(0, 0, 0, 0)

    /*!
      The bottom color of the gradient used for the overlay blending of the
      color that fills the shape. It is optional to set this one as setting
      \l color is enough to set the overall color of the shape.
    */
    property color gradientColor: (Theming.ItemStyle.style && Theming.ItemStyle.style.gradientColor != undefined) ? Theming.ItemStyle.style.gradientColor : Qt.rgba(0, 0, 0, 0)

    /*!
      The image used to mask the \l image.
    */
    property url maskSource: (Theming.ItemStyle.style && Theming.ItemStyle.style.maskSource != undefined) ? Theming.ItemStyle.style.maskSource : ""

    /*!
      The image used as a border.
    */
    property url borderSource: (Theming.ItemStyle.style && Theming.ItemStyle.style.borderSource != undefined) ? Theming.ItemStyle.style.borderSource : ""

    /*!
      The image used to fill the shape.
    */
    property Image image

    width: units.gu(8)
    height: units.gu(8)

    ShaderEffect {
        anchors.fill: parent
        visible: shape.image || shape.color.a != 0.0

        property ShaderEffectSource mask: ShaderEffectSource {
            sourceItem: BorderImage {
                width: shape.width
                height: shape.height
                source: shape.maskSource
                visible: false
            }
        }
        property Image image: shape.image
        property color baseColor: shape.color
        property color gradientColor: shape.gradientColor

        property string colorShader:
            "
            varying highp vec2 qt_TexCoord0;
            uniform lowp float qt_Opacity;
            uniform sampler2D mask;
            uniform lowp vec4 baseColor;
            uniform lowp vec4 gradientColor;

            lowp vec3 blendOverlay(lowp vec3 base, lowp vec3 blend)
            {
                lowp vec3 comparison = clamp(sign(base.rgb - vec3(0.5)), vec3(0.0), vec3(1.0));
                return mix(2.0 * base * blend, 1.0 - 2.0 * (1.0 - base) * (1.0 - blend), comparison);
            }

            void main(void)
            {
                lowp vec4 maskColor = texture2D(mask, qt_TexCoord0.st);

                // this is equivalent to using a vertical linear gradient texture going from vec3(0.0) to vec3(1.0)
                lowp vec4 gradient = gradientColor * qt_TexCoord0.t;

                // FIXME: Because blendOverlay gives incorrect results when we use pre-multiplied alpha,
                // we remove the pre-multiplication before calling blendOverlay, and multiply again afterwards.
                // It works, but is not very elegant.
                lowp vec4 result = vec4(blendOverlay(baseColor.rgb/max(1.0/256.0, baseColor.a), gradient.rgb/max(1.0/256.0, gradient.a)), 1.0);
                result *= baseColor.a;
                gl_FragColor = mix(baseColor, result, gradient.a) * maskColor.a * qt_Opacity;
            }
            "

        property string imageMaskingShader:
            "
            varying highp vec2 qt_TexCoord0;
            uniform lowp float qt_Opacity;
            uniform sampler2D mask;
            uniform sampler2D image;

            void main(void)
            {
                lowp vec4 maskColor = texture2D(mask, qt_TexCoord0.st);
                lowp vec4 imageColor = texture2D(image, qt_TexCoord0.st);
                gl_FragColor = imageColor * maskColor.a * qt_Opacity;
            }
            "

        fragmentShader: image ? imageMaskingShader : colorShader
    }

    BorderImage {
        id: border

        anchors.fill: parent
        source: shape.borderSource
    }
}
