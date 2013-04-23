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

Item {
    id: componentShowcase
    objectName: "componentShowcase"
    width: units.gu(100)
    height: units.gu(75)

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

        currentIndex: 8 // TODO TIM: REVERT TO 0
        model: ListModel {
            ListElement {
                label: "Resolution Independence"
                source: "ResolutionIndependence.qml"
                chapter: "Get Started"
            }
            ListElement {
                label: "Theming"
                source: "Theming.qml"
                chapter: "Style"
            }
            ListElement {
                label: "Ubuntu Shape"
                source: "UbuntuShapes.qml"
                chapter: "Style"
            }
            ListElement {
                label: "Icons"
                source: "Icons.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Header"
                source: "Header.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Toolbars"
                source: "Toolbars.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Switches"
                source: "Switches.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Buttons"
                source: "Buttons.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Tabs"
                source: "Tabs.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Page Stack"
                source: "PageStack.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Dialogs"
                source: ""
                chapter: "Building blocks"
            }
            ListElement {
                label: "List Items"
                source: "ListItems.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Popovers"
                source: "Popups.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Progress"
                source: "ProgressBars.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Scrollbar"
                source: "ScrollBars.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Sheets"
                source: ""
                chapter: "Building blocks"
            }
            ListElement {
                label: "Slider"
                source: "Sliders.qml"
                chapter: "Building blocks"
            }
            ListElement {
                label: "Text Input"
                source: "TextInputs.qml"
                chapter: "Building blocks"
            }
        }
        section.property: "chapter"
        section.labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels

        section.delegate: ListItem.Header { text: section }

        delegate: ListItem.Standard {
            text: i18n.tr(label)
            onClicked: widgetList.currentIndex = index
            enabled: source != ""
            selected: index == widgetList.currentIndex
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
        source: widgetList.model.get(widgetList.currentIndex).source
    }

    Component.onCompleted: {
        i18n.domain = "componentshowcase"
    }
}
