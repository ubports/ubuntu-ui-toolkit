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
                            onClicked: hide()
                        }
                    }
                }
            }
        }

        Component {
            id: defaultSheetComponent
            Sheet {
                id: sheet

                title: "Ain't this the coolest sheet evar?"
                buttonConfiguration: "doneButton"

                Column {
//                    anchors.top: parent.top
                    width: units.gu(30)

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
            id: dialogComponent

            Dialogue {
                id: dialogue

                title: "Save file"
                text: "Are you sure that you want to save this file?"

                Button {
                    text: "cancel"
                    color: "silver"
                    onClicked: dialogue.hide()
                }
                Button {
                    text: "overwrite previous version"
                    color: "orange"
                    onClicked: dialogue.hide()
                }
                Button {
                    text: "save a copy"
                    color: "orange"
                    onClicked: dialogue.hide()
                }
            }
        }

        Component {
            id: buttonComponent
            Column {
                width: units.gu(15)

                Button {
                    id: popButton
                    text: "Pop!"
                    width: parent.width
                    onClicked: PopupUtils.open(popoverComponent, popButton)
                }
//                Button {
//                    id: sheetButton
//                    text: "oh, sheet!"
//                    width: parent.width
//                    onClicked: PopupUtils.open(defaultSheetComponent, sheetButton)
//                }
//                Button {
//                    id: queryButton
//                    text: "Q"
//                    width: parent.width
//                    onClicked: PopupUtils.open(dialogComponent, queryButton)
//                }
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
