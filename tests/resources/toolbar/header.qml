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
import Ubuntu.Components 1.1

MainView {
    id: mainView
    width: units.gu(40)
    height: units.gu(50)
    useDeprecatedToolbar: false

    PageStack {
        id: stack

        Component.onCompleted: stack.push(tabs)
        Tabs {
            selectedTabIndex: 4
            id: tabs
            Tab {
                title: "Stack"
                page: Page {
                    Button {
                        anchors.centerIn: parent
                        text: "Click me"
                        onTriggered: stack.push(pageOnStack)
                    }
                    tools: ToolbarItems {
                        ToolbarButton {
                            action: Action {
                                iconName: "settings"
                                text: "Settings"
                            }
                        }
                    }
                }
            }
            Tab {
                title: "Tools"
                page: Page {
                    Label {
                        anchors.centerIn: parent
                        text: "Second tab"
                    }

                    tools: ToolbarItems {
                        ToolbarButton {
                            action: Action {
                                iconName: "search"
                                text: "Search"
                            }
                        }
                        ToolbarButton {
                            action: Action {
                                iconName: "contact"
                                text: "Contacts"
                            }
                        }
                        ToolbarButton {
                            action: Action {
                                iconName: "share"
                                text: "Share"
                            }
                        }
                        ToolbarButton {
                            action: Action {
                                iconName: "select"
                                text: "Select"
                            }
                        }
                    }
                }
            }
            Tab {
                title: "Switch"
                page: Page {
                    Switch {
                        id: newHeaderSwitch
                        anchors.centerIn: parent
                        checked: !mainView.useDeprecatedToolbar
                        onTriggered: {
                            mainView.useDeprecatedToolbar = !checked;
                        }
                    }
                    Label {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottom: newHeaderSwitch.top
                            bottomMargin: units.gu(1)
                        }
                        text: "Use new header"
                    }
                    tools: ToolbarItems {
                        ToolbarButton {
                            action: Action {
                                iconName: "camera-flip"
                                text: "hello"
                            }
                        }
                    }
                }
            }
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
                                iconName: "search"
                            },
                            Action {
                                iconName: "camera-flip"
                            }
                        ]
                        backAction: Action {
                            iconName: "clear"
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
                        State {
                            // TODO: The definition of this state will be
                            // simplified a lot in a following MR that
                            // introduces the HeaderState component.
                            id: headerState
                            name: "search"
                            property Action back: Action {
                                id: leaveSearchAction
                                text: "back"
                                iconName: "back"
                                onTriggered: searchPage.state = ""
                            }
                            property list<Action> actions: [
                                Action {
                                    iconName: "contact"
                                }
                            ]
                            property TextField input: TextField {
                                placeholderText: "search..."
                            }
                            PropertyChanges {
                                target: searchPage.head
                                backAction: headerState.back
                                actions: headerState.actions
                                input: headerState.input
                            }
                        }
                    ]
                }
            }
        }
    }

    Page {
        id: pageOnStack
        visible: false
        title: "Stacked page"
        Label {
            anchors.centerIn: parent
            text: "Use back button to return"
        }
        tools: ToolbarItems {
            ToolbarButton {
                action: Action {
                    iconName: "search"
                    text: "Search"
                }
            }
        }
    }
}
