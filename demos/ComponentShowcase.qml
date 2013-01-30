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

Rectangle {
    objectName: "componentShowcase"
    width: units.gu(100)
    height: units.gu(75)

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
        objectName: "widgetList"

        width: units.gu(19)
        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        property int selectedIndex: 0
        model: [
            // Get Started
            {"label": i18n.tr("Resolution Independence"), "source": "ResolutionIndependence.qml"},
            // Style
            {"label": i18n.tr("Theming"), "source": "Theming.qml"},
            {"label": i18n.tr("Ubuntu Shape"), "source": "UbuntuShapes.qml"},
            {"label": i18n.tr("Icons"), "source": "GIconProvider.qml"},
            // Building blocks
            // add header component
            {"label": i18n.tr("Toolbars"), "source": ""}, // make this
            {"label": i18n.tr("Switches"), "source": "Switches.qml"}, // Combine with Check Box
            {"label": i18n.tr("Check Box"), "source": "CheckBoxes.qml"}, // (remove)
            {"label": i18n.tr("Buttons"), "source": "Buttons.qml"},
//            {"label": i18n.tr("Tabs (classic)"), "source": "Tabs.qml"},
            {"label": i18n.tr("Tabs"), "source": "NewTabs.qml"},
            {"label": i18n.tr("Page Stack"), "source": "PageStack.qml"}, // Make sure it is called PageStack
            // dialogs
            {"label": i18n.tr("Popups"), "source": "Popups.qml"}, // separate in Popovers, Sheets, Dialogs
            {"label": i18n.tr("List Items"), "source": "ListItems.qml"},
            // popover
            {"label": i18n.tr("Progress"), "source": "ProgressBars.qml"}, //(merge with next)
            {"label": i18n.tr("Activity Indicator"), "source": "ActivityIndicators.qml"}, // integrate in progressbars
            {"label": i18n.tr("Scrollbar"), "source": "ScrollBars.qml"},
            // sheets
            {"label": i18n.tr("Slider"), "source": "Sliders.qml"},
            {"label": i18n.tr("Text Input"), "source": "TextInputs.qml"},
            {"label": i18n.tr("Text Area"), "source": "TextAreas.qml"}, // merge with text input, label single/multi line
        ]

        delegate: ListItem.Standard {
            text: modelData.label
            onClicked: widgetList.selectedIndex = index
            enabled: modelData.source != ""
            selected: index == widgetList.selectedIndex
            showDivider: false
        }
    }

    Scrollbar {
        flickableItem: widgetList
        align: Qt.AlignTrailing
    }

    Loader {
        id: widgetLoader
        objectName: "widgetLoader"

        anchors {
            left: widgetList.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        source: widgetList.model[widgetList.selectedIndex].source
    }

    Component.onCompleted: {
        i18n.domain = "componentshowcase"
    }
}
