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
    anchors.fill: parent

//    Rectangle {
//        anchors.fill: parent
//        color: "pink"
//    }

    height: background.height + separator.height + separatorBottom.height
    Image {
        id: background
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
//            bottom: separator.bottom
        }
        source: Qt.resolvedUrl("artwork/background-paper.png")
        fillMode: Image.Tile
//        height: item.height //units.gu(5)
        height: units.gu(5)
    }

    // FIXME: Define the separator in the theme when this bug is fixed:
    // https://bugs.launchpad.net/goodhope/+bug/1089614
    BorderImage {
        id: separator
        anchors {
//            top: item.tabBar.bottom
            top: background.bottom
            left: parent.left
            right: parent.right
        }
        source: Qt.resolvedUrl("artwork/PageHeaderBaseDividerLight.sci")
    }
    Image {
        id: separatorBottom
        anchors {
            top: separator.bottom
            left: parent.left
            right: parent.right
        }
        source: Qt.resolvedUrl("artwork/PageHeaderBaseDividerBottom.png")
    }


}
