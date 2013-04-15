/*
 * Copyright 2012 Canonical Ltd.
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
import Ubuntu.Components 0.1

Template {
    title: i18n.tr("Toolbars")
    objectName: "Toolbars"

    Item {
        MainView {
            id: mainView
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: units.gu(50)
            height: units.gu(50)

            Tabs {
                id: tabs
                Tab {
                    title: "One"
                    page: Page {
                        Label {
                            anchors.centerIn: parent
                            text: "Simple actions"
                        }
                        tools: ToolbarActions {
                            Action {
                                text: "action 1"
                                iconSource: Qt.resolvedUrl("call_icon.png")
                                onTriggered: print("triggered action 1")
                            }
                            Action {
                                text: "action 2"
                                iconSource: Qt.resolvedUrl("call_icon.png")
                                onTriggered: print("triggered action 2")
                            }
                        }
                    }
                }
                Tab {
                    title: "Two"
                    page: Page {
                        Label {
                            anchors.centerIn: parent
                            text: "Back button, toolbar always visible"
                        }
                        tools: ToolbarActions {
                            Action {
                                text: "action 1"
                                iconSource: Qt.resolvedUrl("call_icon.png")
                                onTriggered: print("one")
                            }
                            Action {
                                text: "action 2"
                                iconSource: Qt.resolvedUrl("call_icon.png")
                                onTriggered: print("two")
                            }
                            back {
                                visible: true
                                onTriggered: tabs.selectedTabIndex--
                            }
                            lock: true
                            active: true
                        }
                    }
                }
                Tab {
                    title: "Three"
                    page: Page {
                        Label {
                            anchors.centerIn: parent
                            text: "Custom back button"
                        }
                        tools: ToolbarActions {
                            Action {
                                text: "action 1"
                                iconSource: Qt.resolvedUrl("call_icon.png")
                            }
                            Action {
                                text: "action 2"
                                iconSource: Qt.resolvedUrl("call_icon.png")
                            }
                            back {
                                itemHint: Button {
                                    id: cancelButton
                                    text: "cancel"
                                }
                                onTriggered: tabs.selectedTabIndex--
                            }
                        }
                    }
                }
            }
        }
    }
}
