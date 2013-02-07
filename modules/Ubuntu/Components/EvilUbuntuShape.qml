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
    \internal
    \qmltype EvilUbuntuShape
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Temporary alternative for \l UbuntuShape that clips the children.
        This class may be deprecated when UbuntuShape is updated.
        It lacks basic features from \l UbuntuShape such as specifying a color or Image.
*/
Item {
    id: shape

    clip: true
    Theming.ItemStyle.class: "UbuntuShape-radius-" + radius

    /*!
      The size of the corners among: "small" (default) and "medium".
    */
    property string radius: "small"

    /*!
      \deprecated
      The image used as a border.
      We plan to expose that feature through styling properties.
    */
    property url borderSource: Theming.ComponentUtils.style(shape, "borderIdle", "")

    implicitWidth: units.gu(8)
    implicitHeight: units.gu(8)

//    default property alias contents: contentsContainer.data
//    Item {
//        id: contentsContainer
//        anchors.fill: parent
//    }

    property alias color: background.color

    property alias item: fbo.sourceItem
    onItemChanged: item.parent = shape

    Theming.Shape {
        anchors.fill: parent
        visible: shape.visible
        image: fbo
        borderSource: shape.borderSource
        radius: shape.radius
    }

    ShaderEffectSource {
        smooth: false // prevent linear interpolation
        id: fbo
        objectName: "FBO"
        hideSource: true
        sourceItem: contentsContainer
//        format: ShaderEffectSource.RGBA
        live: true

        // Do not set visible to false because it will leave the FBO empty,
        //  but position the ShaderEffectSource somewhere that it will be clipped
        //  so it is not visible.
        x: width
        width: parent.width
        height: parent.height
//        width: sourceItem ? sourceItem.width : 0
//        height: sourceItem ? sourceItem.height: 0

        Rectangle {
            id: background
            parent: shape.item ? shape.item: undefined
            anchors.fill: parent ? parent: undefined
            color: "transparent"
        }
    }
}
