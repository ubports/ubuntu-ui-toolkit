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
            id: tabs
            Tab {
                title: "Colors"
                page: Page {
                    Column {
                        height: childrenRect.height
                        width: parent.width - units.gu(8)
                        spacing: units.gu(1)
                        anchors.centerIn: parent

                        Button {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            text: "Light"
                            onTriggered: {
                                mainView.backgroundColor = "#eeeeee";
                                mainView.headerColor = mainView.backgroundColor;
                                mainView.footerColor = mainView.backgroundColor;
                            }
                        }
                        Button {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            text: "Dark"
                            onTriggered: {
                                mainView.backgroundColor = "#333333";
                                mainView.headerColor = mainView.backgroundColor;
                                mainView.footerColor = mainView.backgroundColor;
                            }
                        }
                        Button {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            text: "Gradient"
                            onTriggered: {
                                mainView.backgroundColor = "#6A69A2";
                                mainView.headerColor ="#343C60";
                                mainView.footerColor = "#8896D5";
                            }
                        }
                    }
                }
            }

            Tab {
                title: "Stack"
                page: Page {
                    Button {
                        anchors.centerIn: parent
                        text: "Click me"
                        onTriggered: stack.push(pageOnStack)
                    }
                    head {
                        actions: Action {
                            iconName: "settings"
                            text: "Settings"
                        }
                        foregroundColor: UbuntuColors.purple
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
                                iconName: "settings"
                                text: "Settings"
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

    Page {
        id: pageOnStack
        visible: false
        title: "Stacked page"
        Label {
            anchors.centerIn: parent
            text: "Use back button to return"
        }
        head.actions: Action {
            iconName: "settings"
            text: "settings"
        }
    }
}
