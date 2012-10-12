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
    id: gallery
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
        model: [{"label": i18n.tr("Buttons"), "source": "Buttons.qml"},
                {"label": i18n.tr("Tabs (Segmented)"), "source": "Tabs.qml"},
                {"label": i18n.tr("List Items"), "source": "ListItems.qml"},
                {"label": i18n.tr("Page Stack"), "source": "PageStack.qml"},
                {"label": i18n.tr("Switch"), "source": "Switches.qml"},
                {"label": i18n.tr("Check Box"), "source": "CheckBoxes.qml"},
                {"label": i18n.tr("Activity Indicator"), "source": "ActivityIndicators.qml"},
                {"label": i18n.tr("Progress Bar"), "source": "ProgressBars.qml"},
                {"label": i18n.tr("Slider"), "source": "Sliders.qml"},
                {"label": i18n.tr("Text Input"), "source": "TextInputs.qml"},
                {"label": i18n.tr("Scrollbar"), "source": ""},
                {"label": i18n.tr("Popovers"), "source": ""},
                {"label": i18n.tr("Toolbars"), "source": ""},
                {"label": i18n.tr("Sheets & Dialogues"), "source": ""},
                {"label": i18n.tr("Grid View"), "source": ""},
                {"label": i18n.tr("On Screen Keyboard"), "source": ""},
                {"label": i18n.tr("Date Picker"), "source": ""},
                {"label": i18n.tr("Time Picker"), "source": ""},
                {"label": i18n.tr("Tabs (Non Segmented)"), "source": ""},
               ]

        delegate: ButtonWithForeground {
            height: 48
            textSize: "medium"
            text: modelData.label
            textColor: "#757373"
            onClicked: widgetList.selectedIndex = index
            enabled: modelData.source != ""

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
        source: widgetList.model[widgetList.selectedIndex].source
    }

    Component.onCompleted: {
        i18n.domain = "gallery"
//        i18n.localeDir = "/usr/share/locale"; // default
    }
}
