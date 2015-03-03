/*
 * Copyright 2013 Canonical Ltd.
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

//![0]
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Layouts 1.0

Item {
    id: root
    width: units.gu(30)
    height: units.gu(30)

    Layouts {
        objectName: "layouts"
        id: layouts
        anchors.fill: parent

        layouts: [
            //![flow layout]
            ConditionalLayout {
                name: "flow"
                when: layouts.width > units.gu(70)
                Flow {
                    anchors.fill: parent
                    ItemLayout {
                        item: "red"
                        width: units.gu(20)
                        height: units.gu(20)
                    }
                    ItemLayout {
                        item: "green"
                        width: units.gu(20)
                        height: units.gu(20)
                    }
                    ItemLayout {
                        item: "blue"
                        width: units.gu(20)
                        height: units.gu(20)
                    }
                    Button {
                        text: "Flow item"
                        width: units.gu(20)
                        height: units.gu(20)
                    }
                }
            }
            //![flow layout]
            ,
            //![overlay layout]
            ConditionalLayout {
                name: "row"
                when: layouts.width > units.gu(50)
                Row {
                    anchors.fill: parent
                    ItemLayout {
                        item: "red"
                        width: parent.width / 2
                        height: units.gu(20)

                        Rectangle {
                            anchors{
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                                margins: units.dp(3)
                            }
                            radius: units.dp(1)
                            height: units.gu(8)
                            color: "#bbbbbb88"
                            Label {
                                anchors.centerIn: parent
                                text: "Overlay item"
                            }
                        }
                    }
                    ItemLayout {
                        item: "green"
                        width: parent.width / 2
                        height: units.gu(20)
                    }
                }
            }
            //![overlay layout]
        ]
        //![default layout]
        Button {
            id: redButton
            text: "Item #1"
            color: "red"
            Layouts.item: "red"
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: units.gu(15)
        }
        Button {
            id: greenButton
            text: "Item #2"
            color: "green"
            Layouts.item: "green"
            anchors {
                top: parent.top
                left: redButton.right
                right: parent.right
            }
            height: units.gu(10)
        }
        Button {
            id: blueButton
            text: "Item #3"
            color: "blue"
            Layouts.item: "blue"
            anchors{
                left: redButton.right
                right: parent.right
                bottom: parent.bottom
            }
            height: units.gu(10)
        }
        //![default layout]
    }
}
//![0]
