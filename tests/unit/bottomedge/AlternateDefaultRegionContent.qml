/*
 * Copyright 2015 Canonical Ltd.
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
 *
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: holder
    width: units.gu(40)
    height: units.gu(71)

    BottomEdge {
        id: bottomEdge
        hint.text: "Test"
        objectName: "testItem"
        contentComponent: Rectangle {
            objectName: "defaultContent"
            width: holder.width
            height: holder.height
            color: UbuntuColors.green
        }

        Component {
            id: altContent
            Rectangle {
                objectName: "regionContent"
                width: holder.width - units.gu(20)
                height: holder.height
                color: UbuntuColors.red
            }
        }
    }

    Component.onCompleted: {
        var defaultRegion = bottomEdge.regions[0];
        defaultRegion.contentComponent = altContent;
    }
}

