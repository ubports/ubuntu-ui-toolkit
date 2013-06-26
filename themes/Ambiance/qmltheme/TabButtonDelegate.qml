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

Item {
    id: visuals
    // styling properties
    property url backgroundLeft: styledItem.selected ? "artwork/TabLeftSelected.sci" : "artwork/TabLeftUnselected.sci"
    property url backgroundRight: styledItem.selected ? "artwork/TabRightSelected.sci" : "artwork/TabRightUnselected.sci"
    property url backgroundMiddle: styledItem.selected ? "artwork/TabMiddleSelected.sci" : "artwork/TabMiddleUnselected.sci"

    anchors.fill: parent

    BorderImage {
        anchors.fill: parent
        source: {
            if (tabButton.__isFirst) {
                return visuals.backgroundLeft;
            } else if (tabButton.__isLast) {
                return visuals.backgroundRight;
            } else {
                return visuals.backgroundMiddle;
            }
        }
    }

    TransparentButtonDelegate {
        id: foreground
        // update control's implicitWidth
        Binding {
            target: styledItem
            property: "implicitWidth"
            value: foreground.implicitWidth + 2*foreground.horizontalPadding
        }
    }
}
