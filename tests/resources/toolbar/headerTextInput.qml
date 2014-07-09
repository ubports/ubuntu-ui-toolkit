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

MainView {
    width: units.gu(40)
    height: units.gu(50)
    useDeprecatedToolbar: false

    Page {
        id: searchPage
        title: "Click the icon"

        Label {
            anchors.centerIn: parent
            text: searchPage.state == "search" ? "search mode" : "normal mode"
        }

        head.actions: Action {
            id: searchAction
            iconName: "search"
            onTriggered: searchPage.state = "search"
        }

        state: ""
        states: [
            State {
                name: ""
                PropertyChanges {
                    target: searchPage.head
                    // needed otherwise actions will not be
                    // returned to its original state.
                    actions: [ searchAction ]
                }
            },
            PageHeadState {
                id: headerState
                name: "search"
                head: searchPage.head
                actions: [
                    Action {
                        iconName: "contact"
                    }
                ]
                backAction: Action {
                    id: leaveSearchAction
                    text: "back"
                    iconName: "back"
                    onTriggered: searchPage.state = ""
                }
                contents: TextField {
                    placeholderText: "search..."
                }
            }
        ]
    }
}
