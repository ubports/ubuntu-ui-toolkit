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
    id: mainView
    width: units.gu(40)
    height: units.gu(50)
    useDeprecatedToolbar: true

    PageStack {
        id: stack

        Component.onCompleted: stack.push(tabs)
        Tabs {
            id: tabs
            Tab {
                title: "Tab 1"
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
                title: "Tab 2"
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
                title: "Tab 3"
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
                title: "Tab 4"
                page: Page {
                    Label {
                        anchors.centerIn: parent
                        text: "New API"
                    }
//                    header.actions: [
//                        Action {
//                            iconName: "search"
//                        },
//                        Action {
//                            iconName: "camera-flip"
//                        }
//                    ]
                }
            }
            Tab {
                title: "Tab 5"
                page: Page { }
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
