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
                id: row
                width: parent.width
                //use the max height of the components + 2gu margin per side
                height: Math.max(labelsContainer.height, Math.max(Math.max(item1.height, item2.height), item3.height)) + units.gu(4)
                MouseArea {
                    id: slot1
                    height: parent.height
                    width: item1.width + units.gu(3)
                    Item { id: item1; anchors.centerIn: parent; width: units.gu(2) }
                }
                Item {
                    id: labelsContainer
                    height: title.height + subtitle.height + subsubtitle.height + units.gu(2)
                    width: parent.width - slot1.width - slot2.width - slot3.width
                    anchors.verticalCenter: parent.verticalCenter
                    //Item { id: title; width: parent.width; }
                    //Item { width: parent.width; anchors.top: title.bottom }
                    Text { id: title; width: parent.width; text: "test"; font.weight: Font.Light; color: "#525252"; font.pixelSize: FontUtils.sizeToPixels("medium") }
                    Text { id: subtitle
                        anchors.top: title.baseline;
                        anchors.topMargin: units.gu(1);
                        width: parent.width;
                        text: "test";
                        font.weight: Font.Light;
                        color: "#525252";
                        font.pixelSize: FontUtils.sizeToPixels("small")
                    }
                    Text { id: subsubtitle
                        anchors.top: subtitle.baseline;
                        anchors.topMargin: units.gu(1);
                        width: parent.width;
                        text: "test";
                        font.weight: Font.Light;
                        color: "#525252";
                        font.pixelSize: FontUtils.sizeToPixels("small")
                    }
                }
                MouseArea {
                    id: slot2
                    height: parent.height
                    width: item2.width + units.gu(3)
                    Item { id: item2; anchors.centerIn: parent; width: units.gu(2) }
                }

                MouseArea {
                    id: slot3
                    height: parent.height
                    width: item3.width + units.gu(3)
                    Item { id: item3; anchors.centerIn: parent; width: units.gu(2) }
                }
            }
        }
    }
}