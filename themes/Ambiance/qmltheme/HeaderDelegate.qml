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
    property real contentHeight: StyleUtils.itemStyleProperty("contentHeight", units.gu(7.5))

    property color backgroundColor: StyleUtils.itemStyleProperty("backgroundColor", "transparent")
    property url backgroundSource: itemStyle.backgroundSource
    property url separatorSource: itemStyle.separatorSource
    property url separatorBottomSource: itemStyle.separatorBottomSource

    property int fontWeight: StyleUtils.itemStyleProperty("fontWeight")
    property string fontSize: StyleUtils.itemStyleProperty("fontSize", "x-large")
    property color textColor: StyleUtils.itemStyleProperty("textColor", "black")
    property real textLeftMargin: itemStyle.textLeftMargin

    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        top: parent ? parent.top : undefined
    }
    height: headerDelegate.contentHeight + separator.height + separatorBottom.height

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
            sourceComponent: header.contents
            anchors.fill: parent
        }
    }
}
