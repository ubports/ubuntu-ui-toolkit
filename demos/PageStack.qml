/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 1.1
import Ubuntu.Components 0.1

Template {
    title: "PageStack"


    PageStack {
        id: pageStack
        anchors.fill: parent
        //            Component.onCompleted: push(firstPage)

        Page {
            id: firstPage
            title: "Optional title"
            contents: Column {
                anchors.centerIn: parent
                Text {
                    text: "This is the root page!"
                }
                Row {
                    Button {
                        text: "Orange"
                        onClicked: pageStack.push(orange)
                    }
                    Button {
                        text: "blue"
                        onClicked: pageStack.push(blue)

                    }
                }
            }

            Page {
                id: orange
                title: "Orange"
                contents: Rectangle {
                    color: "orange"
                    width: 250
                    height: 250
                    anchors.centerIn: parent
                    Button {
                        text: "go back"
                        anchors.centerIn: parent
                        onClicked: pageStack.pop()
                    }
                }
            }

            Page {
                id: blue
                title: "Blue"
                contents: Rectangle {
                    id: pink
                    color: "darkblue"
                    radius: 5
                    width: 150
                    height: 150
                    anchors.centerIn: parent
                    Button {
                        text: "back"
                        anchors.centerIn: parent
                        onClicked: pageStack.pop()
                        darkBorder: true
                    } // button
                } // rectangle
            }
        }
    }
}
