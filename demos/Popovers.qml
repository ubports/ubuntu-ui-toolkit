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
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    title: "Popovers"

    Rectangle {
        color: "transparent"
        anchors.fill: parent
        id: canvas


        Component {
            id: popoverComponent

            Popover {
                id: popover
                Column {
                    // TODO: use the column container in one of the subclasses of Popover.
                    // It is too specific to use it here, but right now I need it for the margins.
                    id: containerLayout
                    property real totalHeight: height + anchors.topMargin + anchors.bottomMargin
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
                            onClicked: popover.destroy()
                        }
                    }
                }
            }
        }

        Component {
            id: buttonComponent
            Button {
                id: theActualButton
                text: "Pop!"
                width: 100
                onClicked: {
                    var popover = popoverComponent.createObject(root);
                    popover.caller = theActualButton;
                    popover.visible = true;
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
