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
    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
    }
    height: background.height + separatorBottom.height

    Image {
        id: background
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        source: itemStyle.backgroundSource
        fillMode: Image.Tile
        height: itemStyle.contentHeight + separator.height
    }
    BorderImage {
        id: separator
        anchors {
            bottom: background.bottom
            left: parent.left
            right: parent.right
        }
        source: itemStyle.separatorSource
    }
    Image {
        id: separatorBottom
        anchors {
            top: separator.bottom
            left: parent.left
            right: parent.right
        }
        source: itemStyle.separatorBottomSource
    }

    Item  {
        id: foreground
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: StyleUtils.itemStyleProperty("contentHeight", units.gu(7.5))

        Label {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: itemStyle.margins
            }
            text: item.title
            font.weight: StyleUtils.itemStyleProperty("fontWeight")
            visible: contentsLoader.status != Loader.Ready
        }

        // Link to item here, because Loader has its own item property
        property Header header: item
        Loader {
            id: contentsLoader
            sourceComponent: header.contents
            anchors.fill: foreground
        }
    }
}
