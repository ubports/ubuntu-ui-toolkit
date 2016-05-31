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

import QtQuick 2.0
import Ubuntu.Components 1.3

Column {
    width: 800
    height: 600

    Repeater {
        model: 5000
        ListItem {
            trailingActions: ListItemActions {
                actions: [
                    Action {}
                ]
            }
            leadingActions: ListItemActions {
                actions: [
                    Action {},
                    Action {},
                    Action {}
                ]
            }

            Row {
                anchors.fill: parent

                MouseArea {
                    id: firstContainer
                    height: parent.height
                    width: units.gu(4)
                }
                MouseArea {
                    id: secondContainer
                    height: parent.height
                    width: parent.width - firstContainer.width - thirdContainer.width - fourthContainer.width
                }
                MouseArea {
                    height: parent.height
                    id: thirdContainer
                    width: units.gu(4)
                }
                MouseArea {
                    id: fourthContainer
                    height: parent.height
                    width: units.gu(4)
                }
            }
        }
    }
}
