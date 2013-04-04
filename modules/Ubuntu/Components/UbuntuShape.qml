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
    property color gradientColor: Qt.rgba(0, 0, 0, 0)

    /*!
      \deprecated
      The image used to mask the \l image.
      We plan to expose that feature through styling properties.
    */
    property url maskSource: ""

    /*!
      \deprecated
      The image used as a border.
      We plan to expose that feature through styling properties.
    */
    // the property styling is not visible for the QML parser, therefore this
    // property will be modified only if the value set upon component use differs
    // from the default value; therefore we must set a default value, which does
    // not disturbe the component functionality
    property url borderSource: "*"

    /*!
      The image used to fill the shape.
    */
    property Image image

    implicitWidth: units.gu(8)
    implicitHeight: units.gu(8)

    Theming.Shape {
        anchors.fill: parent
        visible: shape.visible
        image: shape.image && (shape.image.status == Image.Ready) ? shape.image : null
        baseColor: shape.color
        gradientColor: shape.gradientColor
        borderSource: shape.borderSource
        radius: shape.radius
        stretched: shape.image && (shape.image.fillMode == Image.PreserveAspectCrop) ? false : true
        horizontalAlignment: shape.image && (shape.image.horizontalAlignment == Image.AlignLeft) ? Theming.Shape.AlignLeft : shape.image && (shape.image.horizontalAlignment == Image.AlignRight) ? Theming.Shape.AlignRight : Theming.Shape.AlignHCenter
        verticalAlignment: shape.image && (shape.image.verticalAlignment == Image.AlignTop) ? Theming.Shape.AlignTop : shape.image && (shape.image.verticalAlignment == Image.AlignBottom) ? Theming.Shape.AlignBottom : Theming.Shape.AlignVCenter
    }
}
