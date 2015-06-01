/*
 * Copyright 2012-2015 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

MainView {
    width: 800
    height: 600
    id: mainView

    Tabs {
        id: tabs
        selectedTabIndex: 0
        Item {
            // does this mess up stuff? nope.
        }

        Tab {
            id: simpleTab
            title: i18n.tr("Simple page #" + index + " to demonstrate tabs")
            page: Page {
                Label {
                    anchors.centerIn: parent
                    text: "Use tabs drawer to navigate"
                }
                head.actions: Action {
                    text: "action"
                    iconSource: "call_icon.png"
                    onTriggered: print("action triggered")
                }
            }
        }
        Repeater {
            model: 3
            Tab {
                id: tab
                title: "Repeater #" + tab.index
                page: Page {
                    Column {
                        anchors.centerIn: parent
                        width: units.gu(40)
                        Label {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            text: "Repeater tab number "+index
                        }
                        Button {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            text: "Previous"
                            onClicked: if (tabs.selectedTabIndex > 0) tabs.selectedTabIndex--
                        }
                    }
                }
            }
        }
        Tab {
            id: externalTab
            title: i18n.tr("External #" + index)
            page: Loader {
                parent: externalTab
                anchors.fill: parent
                source: (tabs.selectedTab === externalTab) ? Qt.resolvedUrl("MyCustomPage.qml") : ""
            }
        }
        Tab {
            title: i18n.tr("List view #" + index)
            page: Page {
                ListView {
                    clip: true
                    anchors.fill: parent
                    model: 20
                    delegate: ListItem.Standard {
                        iconSource: Qt.resolvedUrl("call_icon.png")
                        text: "Item "+modelData
                    }
                }
            }
        }
    }
}
