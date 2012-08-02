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

import QtQuick 1.1
import Ubuntu.Components 0.1

Rectangle {
    width: 800
    height: 600

    color: "#e6e6e6"

    Rectangle {
        anchors.fill: widgetList
        color: "#cccccc"
    }

    Rectangle {
        width: 1
        anchors {
            right: widgetList.right
            top: widgetList.top
            bottom: widgetList.bottom
        }
        color: "#efefef"
    }

    ListView {
        id: widgetList

        width: 150
        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        property int selectedIndex: 0

        model: ["Buttons", "Tabs", "ListItems", "PageStack", "Drilldown"]

        delegate: ButtonWithForeground {
            text: modelData
            textColor: "#757373"
            onClicked: widgetList.selectedIndex = index

            Rectangle {
                anchors.fill: parent
                anchors.margins: 1
                z: -1
                color: "#e6dede"
                visible: index == widgetList.selectedIndex
            }
        }
    }

    Loader {
        id: widgetLoader

        anchors {
            left: widgetList.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        source: widgetList.model[widgetList.selectedIndex] + ".qml"
    }
}
