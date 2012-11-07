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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    title: "Popovers"

    Rectangle {
        color: "transparent"
        id: canvas

        Component {
            id: popoverComponent

            Popover {
                id: popover
                Column {
                    id: containerLayout
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }

                    ListItem.Header { text: "Share" }

                    ListItem.Standard {
                        text: "Do something"
                    }
                    ListItem.Standard {
                        text: "Do something else"
                    }

                    ListItem.Header { text: "Paste into app" }
                    ListItem.SingleControl {
                        highlightWhenPressed: false
                        control: Button {
                            text: "Do nothing"
                            anchors {
                                fill: parent
                                margins: units.gu(1)
                            }
                        }
                    }
                    ListItem.SingleControl {
                        highlightWhenPressed: false
                        control: Button {
                            text: "Cancel! Abort! Break! NOOOOO"
                            anchors {
                                fill: parent
                                margins: units.gu(1)
                            }
                            onClicked: popover.hide()
                        }
                    }
                }
            }
        }

        Component {
            id: defaultSheetComponent
            Sheet {
                id: sheet

                Column {
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }

                    TextCustom { text: "hello there" }
                    Rectangle {
                        color: "red"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: units.gu(4)
                    }
                    Rectangle {
                        color: "yellow"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: units.gu(4)
                    }
                    Rectangle {
                        color: "green"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: units.gu(4)
                    }
                }
            }
        }

        Component {
            id: buttonComponent
            Column {
                width: 120

                Button {
                    id: theActualButton
                    text: "Pop!"
                    width: parent.width
                    onClicked: {
                        PopupUtils.open(popoverComponent, theActualButton);
                    }
                }
                Button {
                    id: theOtherButton
                    text: "oh, sheet!"
                    width: parent.width
                    onClicked: PopupUtils.open(defaultSheetComponent, theOtherButton);
                }
            }
        }
        Component.onCompleted: {
            var i;
            var b = [];
            for (i=0; i < 9; i++) b[i] = buttonComponent.createObject(canvas);
            for (i=0; i < 3; i++) b[i].anchors.top = canvas.top;
            for (i=3; i < 6; i++) b[i].anchors.verticalCenter = canvas.verticalCenter;
            for (i=6; i < 9; i++) b[i].anchors.bottom = canvas.bottom;

            for (i=0; i < 3; i++) {
                b[3*i].anchors.left = canvas.left;
                b[1+3*i].anchors.horizontalCenter = canvas.horizontalCenter;
                b[2+3*i].anchors.right = canvas.right;
            }
        }
    }
}
