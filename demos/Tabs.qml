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
    title: i18n.tr("Tabs")

    Tabs {
        selectedTabIndex: 1
//        ItemStyle.class: "sliding-tabs"
        Tab {
            title: i18n.tr("Tab") + " 1"
            page: Rectangle {
                anchors.fill: parent
                color: "#eeeeee"
                TextCustom {
                    anchors.centerIn: parent
                    text: i18n.tr("This is the first tab.")
                    color: "#757373"
                }
            }
        }
        Tab {
            iconSource: "call_icon.png"
            page: Rectangle {
                anchors.fill: parent
                color: "tan"
                TextCustom {
                    anchors.centerIn: parent
                    text: i18n.tr("This is the second tab.")
                    color: "#757373"
                }
            }
        }
        Tab {
            title: i18n.tr("Tab") + " 3"
            iconSource: "call_icon.png"
            page: Qt.resolvedUrl("MyCustomPage.qml")
        }
    }
}
