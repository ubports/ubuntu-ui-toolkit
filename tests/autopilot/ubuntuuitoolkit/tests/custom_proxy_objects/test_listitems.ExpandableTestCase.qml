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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1


MainView {
    width: units.gu(48)
    height: units.gu(60)


    Column {
        anchors.fill: parent

        Repeater {
            model: 2
            Expandable {
                id: expandingItem
                objectName: "expandable" + index
                expandedHeight: contentCol.height + units.gu(1)

                onClicked: {
                    expanded = !expanded;
                }

                Column {
                    id: contentCol
                    anchors { left: parent.left; right: parent.right }
                    Item {
                        anchors { left: parent.left; right: parent.right}
                        height: expandingItem.collapsedHeight
                        Label {
                            anchors {
                                left: parent.left
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                            text: "Item " + index
                        }
                    }

                    UbuntuShape {
                        objectName: "shape" + index
                        anchors { left: parent.left; right: parent.right }
                        height: units.gu(6)
                    }
                }
            }
        }
    }
}
