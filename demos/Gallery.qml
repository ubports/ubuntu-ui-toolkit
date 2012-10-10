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

    // Using gallery.tr(text) instead of i18n.tr(text) as a workaround
    // for not automatically re-evaluating strings when i18n.domain/localeDir is updated.
    // Inspired by: https://bugreports.qt-project.org/browse/QTBUG-15602
    function tr(text) { return i18n.tr(text) + gallery.emptyString; }
    property string emptyString: ""

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
        model: [{"label": gallery.tr("Buttons"), "source": "Buttons.qml"},
                {"label": gallery.tr("Tabs (Segmented)"), "source": "Tabs.qml"},
                {"label": gallery.tr("List Items"), "source": "ListItems.qml"},
                {"label": gallery.tr("Page Stack"), "source": "PageStack.qml"},
                {"label": gallery.tr("Switch"), "source": "Switches.qml"},
                {"label": gallery.tr("Check Box"), "source": "CheckBoxes.qml"},
                {"label": gallery.tr("Activity Indicator"), "source": "ActivityIndicators.qml"},
                {"label": gallery.tr("Progress Bar"), "source": "ProgressBars.qml"},
                {"label": gallery.tr("Slider"), "source": "Sliders.qml"},
                {"label": gallery.tr("Text Input"), "source": "TextInputs.qml"},
                {"label": gallery.tr("Scrollbar"), "source": ""},
                {"label": gallery.tr("Popovers"), "source": ""},
                {"label": gallery.tr("Toolbars"), "source": ""},
                {"label": gallery.tr("Sheets & Dialogues"), "source": ""},
                {"label": gallery.tr("Grid View"), "source": ""},
                {"label": gallery.tr("On Screen Keyboard"), "source": ""},
                {"label": gallery.tr("Date Picker"), "source": ""},
                {"label": gallery.tr("Time Picker"), "source": ""},
                {"label": gallery.tr("Tabs (Non Segmented)"), "source": ""},
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
        i18n.localeDir = "/usr/share/locale";
        // trigger a re-evaluation of all strings encapsulated by gallery.tr()
        emptyString = "a";
        emptyString = "";
    }
}
