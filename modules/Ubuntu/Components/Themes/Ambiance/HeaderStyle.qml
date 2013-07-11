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
    id: headerStyle
    /*!
      The height of the headercontents, which is the full height of
      the header minus the separators shown at the bottom of it.
     */
    property real contentHeight: units.gu(7.5)

    /*!
      The background color of the header.
     */
    property color backgroundColor: Theme.palette.normal.background

    /*!
      The source of the background image of the header. If the image is has (semi-)transparent
      pixels, \l backgroundColor can be set to specify the color behind those (semi-)transparent
      pixels.
     */
    property url backgroundSource: "artwork/background_paper.png"

    /*!
      The source of the image that separates the header from the contents of a \l MainView.
      The separator will be drawn over the contents.
     */
    property url separatorSource: "artwork/PageHeaderBaseDividerLight.sci"

    /*!
      The source of an additional image attached to the bottom of the separator. The contents
      of the \l MainView will be drawn on top of the separator bottom image.
     */
    property url separatorBottomSource: "artwork/PageHeaderBaseDividerBottom.png"

    property int fontWeight: Font.Light
    property string fontSize: "x-large"
    property color textColor: Theme.palette.selected.backgroundText
    property real textLeftMargin: units.gu(2)

    implicitHeight: headerStyle.contentHeight + separator.height + separatorBottom.height

    Rectangle {
        id: backgroundColor
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        color: headerStyle.backgroundColor
        visible: color != "transparent"
        height: headerStyle.contentHeight + separator.height
    }
    Image {
        id: background
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        source: headerStyle.backgroundSource
        visible: source != ""
        fillMode: Image.Tile
        height: headerStyle.contentHeight + separator.height
    }
    BorderImage {
        id: separator
        anchors {
            bottom: background.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorSource
    }
    Image {
        id: separatorBottom
        anchors {
            top: separator.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorBottomSource
    }

    Item {
        id: foreground
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: headerStyle.contentHeight

        Label {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: headerStyle.textLeftMargin
            }
            text: styledItem.title
            font.weight: headerStyle.fontWeight
            visible: contentsLoader.status != Loader.Ready
            fontSize: headerStyle.fontSize
            color: headerStyle.textColor
        }

        property Header header: styledItem
        Loader {
            id: contentsLoader
            sourceComponent: foreground.header.contents
            anchors.fill: parent
        }
    }
}
