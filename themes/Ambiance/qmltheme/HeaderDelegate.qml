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
    id: headerDelegate
    /*!
      The height of the headercontents, which is the full height of
      the header minus the separators shown at the bottom of it.
     */
    property real contentHeight: units.gu(7.5)

    /*!
      The background color of the header.
     */
    property color backgroundColor: "transparent"

    /*!
      The source of the background image of the header. If the image is has (semi-)transparent
      pixels, \l backgroundColor can be set to specify the color behind those (semi-)transparent
      pixels.
     */
    property url backgroundSource: ""

    /*!
      The source of the image that separates the header from the contents of a \l MainView.
      The separator will be drawn over the contents.
     */
    property url separatorSource: ""

    /*!
      The source of an additional image attached to the bottom of the separator. The contents
      of the \l MainView will be drawn on top of the separator bottom image.
     */
    property url separatorBottomSource: ""

    property int fontWeight
    property string fontSize: "x-large"
    property color textColor: "black"
    property real textLeftMargin

    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        top: parent ? parent.top : undefined
    }
    height: headerDelegate.contentHeight + separator.height + separatorBottom.height
    Binding {
        target: item
        property: "height"
        value: height
    }

    Rectangle {
        id: backgroundColor
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        color: headerDelegate.backgroundColor
        visible: color != "transparent"
        height: headerDelegate.contentHeight + separator.height
    }
    Image {
        id: background
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        source: headerDelegate.backgroundSource
        visible: source != ""
        fillMode: Image.Tile
        height: headerDelegate.contentHeight + separator.height
    }
    BorderImage {
        id: separator
        anchors {
            bottom: background.bottom
            left: parent.left
            right: parent.right
        }
        source: headerDelegate.separatorSource
    }
    Image {
        id: separatorBottom
        anchors {
            top: separator.bottom
            left: parent.left
            right: parent.right
        }
        source: headerDelegate.separatorBottomSource
    }

    Item {
        id: foreground
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: headerDelegate.contentHeight

        Label {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: headerDelegate.textLeftMargin
            }
            text: item.title
            font.weight: headerDelegate.fontWeight
            visible: contentsLoader.status != Loader.Ready
            fontSize: headerDelegate.fontSize
            color: headerDelegate.textColor
        }

        // Link to item here, because Loader has its own item property
        property Header header: item
        Loader {
            id: contentsLoader
            sourceComponent: foreground.header.contents
            anchors.fill: parent
        }
    }
}
