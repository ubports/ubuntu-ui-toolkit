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
    // styling properties
    anchors.fill: parent
    z: -1
    id: mainViewDelegate

    /*!
      The background color of the main view.
     */
    property color backgroundColor: "#ededf0"

    /*!
      The background texture of the main view. The image will be drawn over the background color,
      so if it has (semi-)transparent pixels, in those pixels the background color will be visible.
     */
    property url backgroundSource: "artwork/background_paper.png"

    Rectangle {
        id: backgroundColor
        anchors.fill: parent
        color: mainViewDelegate.backgroundColor
    }

    Image {
        id: backgroundTexture
        anchors.fill: parent
        source: mainViewDelegate.backgroundSource
        fillMode: Image.Tile
    }
}
