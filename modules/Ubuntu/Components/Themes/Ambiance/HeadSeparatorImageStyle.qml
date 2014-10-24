/*
 * Copyright 2014 Canonical Ltd.
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
import QtQuick 2.2

// This is the old DEPRECATED header separator, which is here for backwards compatibility.
// The new header divider is in HeadDividerStyle.qml.
Item {
    id: separatorStyle
    BorderImage {
        id: separatorImage
        anchors.fill: parent
        source: styledItem.separatorSource
    }
    Image {
        id: separatorBottom
        anchors {
            top: separatorImage.bottom
            left: parent.left
            right: parent.right
        }
        source: styledItem.separatorBottomSource
    }
}
