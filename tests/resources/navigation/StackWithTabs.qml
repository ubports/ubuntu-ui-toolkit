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
import Ubuntu.Components 1.1

MainView {
    id: mainView
    width: units.gu(38)
    height: units.gu(50)
    useDeprecatedToolbar: false

    PageStack {
        id: pageStack
        Component.onCompleted: push(tabs)

        Tabs {
            id: tabs
            Tab {
                title: "Tab 1"
                page: Page {
                    Column {
                        anchors {
                            centerIn: parent
                        }
                        width: childrenRect.width
                        height: childrenRect.height
                        spacing: units.gu(1)

                        Label {
                            text: "Tab bar always in selection mode?"
                        }
                        Switch {
                            id: alwaysSelectionModeSwitch
                            Binding {
                                target: tabs.tabBar
                                property: "alwaysSelectionMode"
                                value: alwaysSelectionModeSwitch.checked
                            }
                        }
                        Label {
                            text: "Animate tab bar."
                        }
                        Switch {
                            id: animateSwitch
                            checked: true
                            Binding {
                                target: tabs.tabBar
                                property: "animate"
                                value: animateSwitch.checked
                            }
                        }
                    }
                }
            }
            Tab {
                title: "Tab 2"
                page: Page {
                    Button {
                        anchors.centerIn: parent
                        onClicked: pageStack.push(page3)
                        text: "Press"
                    }
                }
            }
        }
        Page {
            id: page3
            visible: false
            title: "Page on stack"
            Label {
                anchors.centerIn: parent
                text: "Press back to return to the tabs"
            }
        }
    }
}
