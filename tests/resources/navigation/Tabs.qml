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

MainView {
    id: root
    width: 800
    height: 600

    property var repeaterModel: 3

    Component {
        id: dynamicTab
        Tab {
            page: Page {
                Label {
                    text: title + " at index " + index
                    anchors.centerIn: parent
                }
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "move @1"
                        onTriggered: {
                            print("MOVE TAB TO #1")
                            tabs.moveTab(index, 1)
                        }
                    }
                    ToolbarButton {
                        text: "remove me"
                        onTriggered: {
                            print("REMOVE CURENT TAB")
                            tabs.removeTab(index)
                        }
                    }
                    ToolbarButton {
                        text: "remove first"
                        onTriggered: {
                            print("REMOVE TAB AT #0")
                            tabs.removeTab(0)
                        }
                    }
                }
            }
        }
    }

    Tabs {
        id: tabs
        selectedTabIndex: 0
        Item {
            // does this mess up stuff? nope.
        }

        Tab {
            id: simpleTab
            objectName: title
            title: i18n.tr("Simple page #" + index)
            page: Page {
                title: "This title is not visible"
                Row {
                    anchors.centerIn: parent
                    spacing: units.gu(1)
                    width: childrenRect.width
                    height: childrenRect.height
                    Button {
                        text: "tab bar on"
                        enabled: !tabs.tabBar.selectionMode
                        onClicked: tabs.tabBar.selectionMode = true;
                    }
                    Button {
                        text: "tab bar off"
                        enabled: tabs.tabBar.selectionMode
                        onClicked: tabs.tabBar.selectionMode = false;
                    }
                }
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "action"
                        iconSource: "call_icon.png"
                        onTriggered: print("action triggered")
                    }
                    ToolbarButton {
                        text: "append"
                        onTriggered: {
                            print("APPEND TAB")
                            tabs.addTab("Appended tab", dynamicTab)
                        }
                    }
                    ToolbarButton {
                        text: "insert@1"
                        onTriggered: {
                            print("INSERT TAB TO #1")
                            tabs.insertTab(1, "Inserted tab", dynamicTab)
                        }
                    }
                    ToolbarButton {
                        text: "insert@2"
                        onTriggered: {
                            print("INSERT BETWEEN REPEATERS #1")
                            tabs.insertTab(2, "Between repeaters", dynamicTab)
                        }
                    }
                    ToolbarButton {
                        text: "insert@here"
                        onTriggered: {
                            print("INSERT AFTER ME")
                            tabs.insertTab(simpleTab.index, "Inserted tab", dynamicTab)
                        }
                    }
                    ToolbarButton {
                        text: "incRep"
                        onTriggered: {
                            print("INCREASE REPEATER MODEL")
                            root.repeaterModel += 1
                        }
                    }
                    ToolbarButton {
                        text: "remove last"
                        onTriggered: {
                            print("REMOVE LAST TAB")
                            tabs.removeTab(tabs.count - 1)
                        }
                    }
                    ToolbarButton {
                        text: "append predec"
                        onTriggered: tabs.addTab("Re-added ListView", listViewTab)
                    }
                }
            }
        }
        Repeater {
            model: root.repeaterModel
            Tab {
                id: tab
                objectName: title
                title: "Extra #" + tab.index
                page: Page {
                    Column {
                        anchors.centerIn: parent
                        width: units.gu(40)
                        Label {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            text: "Extra tab number "+index
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
            objectName: title
            title: i18n.tr("External #" + index)
            page: Loader {
                parent: externalTab
                anchors.fill: parent
                source: (tabs.selectedTab === externalTab) ? Qt.resolvedUrl("MyCustomPage.qml") : ""
            }
        }
        Tab {
            id: listViewTab
            objectName: title
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
