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
import QtQuick 2.4
import Ubuntu.Components 1.2

Item {
    // styling properties
    anchors.fill: parent
    z: -1
    id: mainViewStyle

    /*!
      The background texture of the main view. The image will be drawn over the background color,
      so if it has (semi-)transparent pixels, in those pixels the background color will be visible.
     */
    property url backgroundSource: Qt.resolvedUrl("../artwork/background_paper.png")

    Gradient {
        id: backgroundGradient
        GradientStop { position: 0.0; color: styledItem.headerColor }
        GradientStop { position: 0.83; color: styledItem.backgroundColor }
        GradientStop { position: 1.0; color: styledItem.footerColor }
    }

    Rectangle {
        id: backgroundColor
        anchors.fill: parent
        color: styledItem.backgroundColor
        gradient: internals.isGradient ? backgroundGradient : null
        visible: internals.isGradient
    }

    Binding {
        target: typeof window != 'undefined' ? window : null
        property: "color"
        value: styledItem.backgroundColor
    }

    Image {
        id: backgroundTexture
        anchors.fill: parent
        source: mainViewStyle.backgroundSource
        fillMode: Image.Tile
        asynchronous: true
        cache: false
    }

    QtObject {
        id: internals
        property bool isGradient: styledItem.backgroundColor != styledItem.headerColor ||
                                  styledItem.backgroundColor != styledItem.footerColor
    }
}
