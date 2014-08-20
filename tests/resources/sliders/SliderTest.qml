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
import Ubuntu.Components.ListItems 1.0

MainView {
    width: units.gu(50)
    height: units.gu(100)

    Column {
        anchors {
            fill: parent
            margins: units.gu(1)
        }
        UbuntuListView {
            id: listView
            width: parent.width
            height: units.gu(20)
            clip: true
            model: 10
            interactive: true
            delegate: Standard {
                control: Slider {
                    objectName: "testSlider" + index
                }
            }
        }
        UbuntuListView {
            id: listView2
            width: parent.width
            height: units.gu(20)
            clip: true
            model: 10
            delegate: Slider {
                objectName: "testSlider" + index
            }
        }
        Flickable {
            id: flickable
            width: parent.width
            height: units.gu(20)
            clip: true
            contentHeight: column.height
            Column {
                id: column
                width: parent.width
                height: childrenRect.height
                Repeater {
                    model: 10
                    Standard {
                        control: Slider {
                            objectName: "testSlider" + index
                        }
                    }
                }
            }
        }
        Flickable {
            id: flickable2
            width: parent.width
            height: units.gu(20)
            clip: true
            contentHeight: column2.height
            Column {
                id: column2
                width: parent.width
                height: childrenRect.height
                Repeater {
                    model: 10
                    Slider {
                        objectName: "testSlider" + index
                    }
                }
            }
        }
    }
}
