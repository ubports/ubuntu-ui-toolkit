/*
 * Copyright (C) 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1

// FIXME: This example can be much simplified using PageHeadState
//  when bug #1345775 has been fixed.

MainView {
    id: mainView
    width: units.gu(40)
    height: units.gu(50)
    useDeprecatedToolbar: false

    Page {
        id: page
        title: "Demo"

        state: "default"
        states: [
            PageHeadState {
                name: "default"
                head: page.head
                actions: [
                    Action {
                        iconName: "contact"
                        text: "Contact"
                    }
                ]
            },
            State {
                id: selectState
                name: "select"

                property Action leaveSelect: Action {
                    iconName: "back"
                    text: "Back"
                    onTriggered: page.state = "default"
                }
                property list<Action> actions: [
                    Action {
                        iconName: "select"
                        text: "Select All"
                    },
                    Action {
                        iconName: "delete"
                        text: "Delete"
                    }
                ]
                PropertyChanges {
                    target: page.head
                    backAction: selectState.leaveSelect
                    actions: selectState.actions
                    preset: "select"
                }
            }
        ]

        Label {
            anchors.centerIn: parent
            text: "Use back button to leave selection mode."
            visible: page.state == "select"
        }

        Button {
            anchors.centerIn: parent
            onClicked: page.state = "select"
            visible: page.state != "select"
            text: "selection mode"
        }
    }
}
