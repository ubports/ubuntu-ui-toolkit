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

MainView {
    width: units.gu(48)
    height: units.gu(60)

    useDeprecatedToolbar: false

    Page {
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
                        buttonRepeater.itemAt(i).action.visible = false;
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

        tools: ToolbarItems {
            back: ToolbarButton {
                action: Action {
                    id: cancelAction
                    iconName: "cancel"
                    text: "cancel"
                    onTriggered: label.text = "Cancel button clicked."
                }
            }
            Repeater {
                id: buttonRepeater
                model: 5
                ToolbarButton {
                    action: Action {
                        objectName: "action" + index
                        text: "text " + index
                        iconName: "add"
                        onTriggered: label.text = "Button "+index+" clicked."
                    }
                }
            }
        }
    }
}
