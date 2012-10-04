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

Rectangle {
    width: 800
    height: 600

    focus: true
    property var buckets: ["mdpi", "hdpi", "xhdpi"]
    property int bucketNumber: 0
    Keys.onUpPressed: {
        bucketNumber = Math.min(bucketNumber + 1, buckets.length-1)
        units.bucket = buckets[bucketNumber]
    }
    Keys.onDownPressed: {
        bucketNumber = Math.max(bucketNumber - 1, 0)
        units.bucket = buckets[bucketNumber]
    }

    color: "#e6e6e6"

    Rectangle {
        anchors.fill: widgetList
        color: "#cccccc"
    }

    Rectangle {
        width: units.dp(1)
        anchors {
            right: widgetList.right
            top: widgetList.top
            bottom: widgetList.bottom
        }
        color: "#efefef"
    }

    ListView {
        id: widgetList

        width: units.dp(150)
        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        property int selectedIndex: 0
        model: [{"label": "Buttons", "source": "Buttons.qml"},
                {"label": "Tabs (Segmented)", "source": "Tabs.qml"},
                {"label": "List Items", "source": "ListItems.qml"},
                {"label": "Page Stack", "source": "PageStack.qml"},
                {"label": "Switch", "source": "Switches.qml"},
                {"label": "Check Box", "source": "CheckBoxes.qml"},
                {"label": "Activity Indicator", "source": "ActivityIndicators.qml"},
                {"label": "Progress Bar", "source": "ProgressBars.qml"},
                {"label": "Slider", "source": "Sliders.qml"},
                {"label": "Text Input", "source": "TextInputs.qml"},
                {"label": "Scrollbar", "source": ""},
                {"label": "Popovers", "source": ""},
                {"label": "Toolbars", "source": ""},
                {"label": "Sheets & Dialogues", "source": ""},
                {"label": "Grid View", "source": ""},
                {"label": "On Screen Keyboard", "source": ""},
                {"label": "Date Picker", "source": ""},
                {"label": "Time Picker", "source": ""},
                {"label": "Tabs (Non Segmented)", "source": ""},
               ]

        delegate: ButtonWithForeground {
            height: units.dp(48)
            textSize: "medium"
            text: modelData.label
            textColor: "#757373"
            onClicked: widgetList.selectedIndex = index
            enabled: modelData.source != ""

            Rectangle {
                anchors.fill: parent
                anchors.margins: units.dp(1)
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
        source: widgetList.model[widgetList.selectedIndex].source
    }
}
