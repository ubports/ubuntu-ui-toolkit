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
    property bool newTabs: false

    title: newTabs ? i18n.tr("Tabs (new)") : i18n.tr("Tabs (old)")

    Tabs {
        ItemStyle.class: newTabs ? "new-tabs" : ""

        id: tabs
        Tab {
            title: i18n.tr("Tab") + " 1"
            __flickable: flickable
            page: Rectangle {
                anchors.fill: parent
                color: "#eeeeee"

                Flickable {
                    id: flickable
                    clip: true
                    anchors.fill: parent
                    contentHeight: column.height
                    contentWidth: parent.width
                    flickableDirection: Flickable.VerticalFlick

                    Column {
                        id: column
                        width: parent.width
                        height: childrenRect.height

                        Label {
                            text: "\n\n\n\n\n\n\n\n\n\n" +
                                  i18n.tr("This is the first tab.") +
                                  "\n\n\n\n\n\n\n\n\n\n\n\n(" +
                                  i18n.tr("scroll down") + ")\n\n\n"
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Repeater {
                            model: 50
                            Label {
                                text: "Lorem ipsum dolor sit amet, platea est tincidunt nunc, commodo odio elit."
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Label {
                            text: "\n\n\n" + i18n.tr("The end.")
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
        Tab {
            iconSource: "call_icon.png"
            title: i18n.tr("Tab") + " 2"
            page: Rectangle {
                anchors.fill: parent
                color: "tan"
                Row {
                    anchors.centerIn: parent
                    Button {
                        width: units.gu(20)
                        text: i18n.tr("Go to previous tab")
                        onClicked: tabs.selectedTabIndex--
                    }
                    Button {
                        width: units.gu(20)
                        text: i18n.tr("Go to next tab")
                        onClicked: tabs.selectedTabIndex++
                    }
                }
            }
        }
        Tab {
            title: i18n.tr("Tab") + " 3"
            iconSource: "call_icon.png"
            page: Qt.resolvedUrl("MyCustomPage.qml")
        }
        Tab {
            title: i18n.tr("List view")
            __flickable: page
            page: ListView {
                clip: true
                anchors.fill: parent
                model: 20
                delegate: ListItem.Standard {
                    text: "Item "+modelData
                }
            }
        }
    }
}
