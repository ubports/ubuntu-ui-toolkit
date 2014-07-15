/*
 * Copyright (C) 2013-2014 Canonical Ltd.
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
import Ubuntu.Components 1.0

MainView {
    width: units.gu(50)
    height: units.gu(80)
    useDeprecatedToolbar: false

    Action {
        id: action1
        text: "action 1"
        onTriggered: print("one!")
        iconName: "call-start"
    }

    Page {
        title: "This is a long title for the test page"

        Label {
            anchors.centerIn: parent
            text: "Hello, world"
        }

        tools: ToolbarItems {
            Button { // normal-styled button.
                anchors.verticalCenter: parent.verticalCenter
                width: units.gu(10)
                action: Action {
                    text: "yeah"
                    onTriggered: print("upa")
                    iconName: "cancel"
                }
            }
            ToolbarButton {
                action: Action {
                    iconName: "contact"
                    text: "oh"
                    onTriggered: print("lala")
                    enabled: false
                }
            }
            ToolbarButton {
                action: action1
            }
            ToolbarButton {
                action: Action {
                    id: action2
                    onTriggered: print("two!")
                    iconName: "contact"
                    text: "Second action"
                }
                text: "action 2"
            }

            back: ToolbarButton {
                action: Action {
                    text: "cancel"
                    iconName: "cancel"
                    onTriggered: {
                        action2.visible = false;
                        visible = false;
                    }
                }
            }
        }
    }
}
