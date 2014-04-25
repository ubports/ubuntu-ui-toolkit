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
import Ubuntu.Components.ListItems 1.0 as ListItem

Rectangle {
    color: "white"
    width: units.gu(50)
    height: units.gu(70)

MainView {
    id: mainView
    width: units.gu(38)
    height: units.gu(50)
    anchors.centerIn: parent
    useDeprecatedToolbar: false

    PageStack {
        id: pageStack
        Component.onCompleted: push(page0)

        Page {
            id: page0
            title: i18n.tr("Root page")
            visible: false

            Column {
                anchors.fill: parent
                ListItem.Standard {
                    text: i18n.tr("Page one")
                    onClicked: pageStack.push(page1)
                    progression: true
                }
                ListItem.Standard {
                    text: i18n.tr("External page")
                    onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                    progression: true
                }
            }
        }

        Page {
            id: page1
            title: i18n.tr("First page")
            visible: false

            Column {
                anchors.fill: parent
                ListItem.Standard {
                    text: i18n.tr("Root page (again)")
                    onClicked: pageStack.push(page0)
                    progression: true
                }
                ListItem.Standard {
                    text: i18n.tr("Red rectangle")
                    onClicked: pageStack.push(page2, {color: "red"})
                    progression: true
                }
            }

            tools: ToolbarItems {
                ToolbarButton {
                    text: "oh"
                }
            }
        }

        Page {
            title: "Rectangle"
            id: page2
            visible: false
            property alias color: rectangle.color
            Rectangle {
                id: rectangle
                anchors {
                    fill: parent
                    margins: units.gu(5)
                }
            }
        }
    }
}
}
