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
import Ubuntu.Components 0.1
import Ubuntu.Layouts 0.1

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
                name: "flow"
                when: layouts.width > units.gu(70) && layouts.width <= units.gu(90)
                Row {
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
                    id: flow
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
            id: button1
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
            id: button2
            text: "Item #2"
            color: "green"
            Layouts.item: "green"
            anchors {
                top: parent.top
                left: button1.right
                right: parent.right
            }
            height: units.gu(10)
        }
        Button {
            id: nolayout
            text: "Non-laid out"
            color: "brown"
            anchors {
                top: button2.bottom
                left: button1.right
                right: parent.right
                bottom: button3.top
            }
        }
        Button {
            id: button3
            text: "Item #3"
            color: "blue"
            Layouts.item: "blue"
            anchors{
                left: button1.right
                right: parent.right
                bottom: parent.bottom
            }
            height: units.gu(10)
        }
        //![default layout]
    }
    //! [0]
}
