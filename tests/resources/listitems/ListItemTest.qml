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
import Ubuntu.Components 1.1

Item {
    width: units.gu(50)
    height: units.gu(100)

    Column {
        width: parent.width
        ViewItem {
            id: testItem
            background.color: "green"
            onClicked: {
                print("click")
                units.gridUnit += 1;
            }
            Item {
                anchors.fill: parent
            }
        }

        ListView {
            id: view
            clip: true
            width: parent.width
            height: units.gu(40)
            model: 100
            pressDelay: 0
            delegate: ViewItem {
                onClicked: print(" clicked")
                Label {
                    text: modelData + " item"
                }
            }
        }
        Flickable {
            id: flicker
            width: parent.width
            height: units.gu(40)
            clip: true
            contentHeight: column.childrenRect.height
            Column {
                id: column
                width: view.width
                Repeater {
                    model: 100
                    ViewItem {
                        background {
                            color: "red"
                            pressedColor: "lime"
                        }
                        onClicked: divider.visible = !divider.visible
                    }
                }
            }
        }
    }
}
