/*
 * Copyright (C) 2013-2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.2

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(70)

    Page {
        title: "Search"

        id: searchPage

        Label {
            anchors.centerIn: parent
            text: searchPage.state == "search" ? "search mode" : "normal mode"
        }

        state: "default"
        states: [
            PageHeadState {
                name: "default"
                head: searchPage.head
                actions: Action {
                    id: searchAction
                    iconName: "search"
                    onTriggered: searchPage.state = "search"
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
                    onTriggered: searchPage.state = "default"
                }
                contents: TextField {
                    placeholderText: "search..."
                }
            }
        ]
    }
}
