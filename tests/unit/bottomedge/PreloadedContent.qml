/*
 * Copyright 2016 Canonical Ltd.
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
        height: parent.height
        hint.text: "Test"
        objectName: "testItem"
        preloadContent: true
        contentComponent: Rectangle {
            objectName: "default"
            width: bottomEdge.width
            height: bottomEdge.height
            color: UbuntuColors.silk
        }

        BottomEdgeRegion {
            from: 0.2
            to: 0.5
            contentComponent: Rectangle {
                objectName: "region1"
                width: bottomEdge.width - units.gu(10)
                height: bottomEdge.height
                color: UbuntuColors.red
            }
        }
        BottomEdgeRegion {
            from: 0.5
            to: 0.7
            contentComponent: Rectangle {
                objectName: "region2"
                width: bottomEdge.width - units.gu(15)
                height: bottomEdge.height
                color: UbuntuColors.blue
            }
        }
    }
}
