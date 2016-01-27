/*
 * Copyright 2014-2015 Canonical Ltd.
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

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Page {
        id: page
        title: "Test title"

        Flickable {
            anchors.fill: parent
            contentHeight: units.gu(120)
            objectName: "header_test_flickable"

            Label {
                id: label
                objectName: "clicked_label"
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                text: "No button clicked."
            }

            Button {
                objectName: "hide_actions_button"
                anchors {
                    top: label.bottom
                    topMargin: units.gu(5)
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Hide some actions"
                onClicked: {
                    cancelAction.visible = false;
                    for (var i=0; i < 3; i++) {
                        page.head.actions[i].visible = false;
                    }
                    // only three of five visible actions left
                }
            }
            Label {
                id: endLabel
                objectName: "end_label"
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                text: "The end."
            }
        }

        head {
            backAction: Action {
                id: cancelAction
                iconName: "cancel"
                text: "cancel"
                onTriggered: label.text = "Cancel button clicked."
            }
            actions: [
                Action {
                    objectName: "action0"
                    text: "text 0"
                    iconName: "add"
                    onTriggered: label.text = "Button 0 clicked."
                },
                Action {
                    objectName: "action1"
                    text: "text 1"
                    iconName: "add"
                    onTriggered: label.text = "Button 1 clicked."
                },
                Action {
                    objectName: "action2"
                    text: "text 2"
                    iconName: "add"
                    onTriggered: label.text = "Button 2 clicked."
                },
                Action {
                    objectName: "action3"
                    text: "text 3"
                    iconName: "add"
                    onTriggered: label.text = "Button 3 clicked."
                },
                Action {
                    objectName: "action4"
                    text: "text 4"
                    iconName: "add"
                    onTriggered: label.text = "Button 4 clicked."
                }
            ]
        }
    }
}
