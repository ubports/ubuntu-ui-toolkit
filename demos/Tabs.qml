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
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    objectName: "Tabs"
    property bool newTabs: false

    title: i18n.tr("Tabs")

    MainView {
        Tabs {
            id: tabs
            Tab {
                title: i18n.tr("Simple page")
                page: Page {
                    Label {
                        id: label
                        anchors.centerIn: parent
                        text: "A centered label"
                    }
                    tools: ToolbarActions {
                        Action {
                            text: "action"
                            onTriggered: print("action triggered")
                        }
                    }
                }
            }
            Tab {
                id: externalTab
                title: i18n.tr("External")
                iconSource: "call_icon.png"
                page: Loader {
                    parent: externalTab
                    anchors.fill: parent
                    source: (tabs.selectedTab === externalTab) ? Qt.resolvedUrl("MyCustomPage.qml") : ""
                }
            }
            Tab {
                title: i18n.tr("List view")
                page: Page {
                    ListView {
                        clip: true
                        anchors.fill: parent
                        model: 20
                        delegate: ListItem.Standard {
                            icon: Qt.resolvedUrl("avatar_contacts_list.png")
                            text: "Item "+modelData
                        }
                    }
                }
            }
        }
    }
}
