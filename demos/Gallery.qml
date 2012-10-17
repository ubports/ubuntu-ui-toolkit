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

    color: "#e6e6e6"

    function _(text) { return i18n.tr(text); }

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
        model: [{"label": _("Buttons"), "source": "Buttons.qml"},
                {"label": _("Tabs (Segmented)"), "source": "Tabs.qml"},
                {"label": _("List Items"), "source": "ListItems.qml"},
                {"label": _("Page Stack"), "source": "PageStack.qml"},
                {"label": _("Switch"), "source": "Switches.qml"},
                {"label": _("Check Box"), "source": "CheckBoxes.qml"},
                {"label": _("Activity Indicator"), "source": "ActivityIndicators.qml"},
                {"label": _("Progress Bar"), "source": "ProgressBars.qml"},
                {"label": _("Slider"), "source": "Sliders.qml"},
                {"label": _("Text Input"), "source": "TextInputs.qml"},
                {"label": _("Scrollbar"), "source": ""},
                {"label": _("Popovers"), "source": ""},
                {"label": _("Toolbars"), "source": ""},
                {"label": _("Sheets & Dialogues"), "source": ""},
                {"label": _("Grid View"), "source": ""},
                {"label": _("On Screen Keyboard"), "source": ""},
                {"label": _("Date Picker"), "source": ""},
                {"label": _("Time Picker"), "source": ""},
                {"label": _("Tabs (Non Segmented)"), "source": ""},
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
    }
}
