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

import QtQuick 2.3
import Ubuntu.Components 1.2

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(70)

    PageStack {
        id: stack

        Component.onCompleted: stack.push(tabs)
        Tabs {
            id: tabs

            selectedTabIndex: 1
            Tab {
                title: "Actions"
                page: Page {
                    Label {
                        anchors.centerIn: parent
                        text: "New API"
                    }
                    head {
                        actions: [
                            Action {
                                iconName: "settings"
                            },
                            Action {
                                iconName: "camera-flip"
                            }
                        ]
                        backAction: Action {
                            iconName: "close"
                            onTriggered: tabs.selectedTabIndex = 0
                        }
                    }
                }
            }
            Tab {
                title: "Search"
                page: Page {
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
            Tab {
                title: "Sections"
                page: Page {
                    id: sectionsPage
                    head {
                        sections {
                            model: ["navy", "lightblue", "brown"]
                        }
                    }
                    Rectangle {
                        anchors {
                            fill: parent
                            margins: units.gu(2)
                        }
                        color: sectionsPage.head.sections.model[sectionsPage.head.sections.selectedIndex]
                    }
                }
            }
        }
    }
}
