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

import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Layouts 1.0

Item {
    id: root
    width: units.gu(30)
    height: units.gu(30)

    //! [0]
    Layouts {
        id: layouts
        anchors.fill: parent

        layouts: [
            //![column]
            ConditionalLayout {
                name: "column"
                when: layouts.width > units.gu(50) && layouts.width <= units.gu(70)
                Column {
                    anchors.fill: parent
                    ItemLayout {
                        item: "red"
                        height: parent.height / 3
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    ItemLayout {
                        item: "green"
                        height: parent.height / 3
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    ItemLayout {
                        item: "blue"
                        height: parent.height / 3
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                }
            },
            //![column]
            //![row]
            ConditionalLayout {
                name: "row"
                when: layouts.width > units.gu(70) && layouts.width <= units.gu(90)
                Row {
                    anchors.fill: parent
                    ItemLayout {
                        item: "blue"
                        width: parent.width / 3
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }
                    }
                    ItemLayout {
                        item: "red"
                        width: parent.width / 3
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }
                    }
                    ItemLayout {
                        item: "green"
                        width: parent.width / 3
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }
                    }
                }
            },
            //![row]
            //![hiding-element]
            ConditionalLayout {
                name: "hiding-element"
                when: layouts.width > units.gu(90) && layouts.width < units.gu(100)
                Row {
                    anchors.fill: parent
                    ItemLayout {
                        item: "red"
                        width: parent.width / 2
                        height: units.gu(20)
                    }
                    ItemLayout {
                        item: "green"
                        width: parent.width / 2
                        height: units.gu(20)
                    }
                }
            },
            //![hiding-element]
            //![showing-more]
            ConditionalLayout {
                name: "showing-more"
                when: layouts.width >= units.gu(100)
                Flow {
                    anchors.fill: parent
                    ItemLayout {
                        item: "red"
                        width: units.gu(50)
                        height: units.gu(20)
                    }
                    ItemLayout {
                        item: "green"
                        width: units.gu(50)
                        height: units.gu(20)
                    }
                    ItemLayout {
                        item: "blue"
                        width: units.gu(50)
                        height: units.gu(20)
                    }
                    Button {
                        text: "Flow item"
                        width: units.gu(50)
                        height: units.gu(20)
                    }
                }
            }
            //![showing-more]
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
            id: nolayout
            text: "Non-laid out"
            color: "brown"
            anchors {
                top: greenButton.bottom
                left: redButton.right
                right: parent.right
                bottom: blueButton.top
            }
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
    //! [0]
}
