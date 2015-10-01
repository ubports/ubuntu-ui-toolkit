/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    id: mainPage
    title: "Ubuntu UI Toolkit"

    head.actions: [
        Action {
            text: i18n.tr('Right to Left')
            iconName: 'flash-on'
            visible: !gallery.rtl
            onTriggered: gallery.rtl = !gallery.rtl
        },
        Action {
            text: i18n.tr('Left to Right')
            iconName: 'flash-off'
            visible: gallery.rtl
            onTriggered: gallery.rtl = !gallery.rtl
        },
        Action {
            text: i18n.tr('Use dark theme')
            iconName: 'torch-on'
            visible: gallery.theme.name == 'Ubuntu.Components.Themes.Ambiance'
            onTriggered: gallery.theme.name = 'Ubuntu.Components.Themes.SuruDark'
        },
        Action {
            text: i18n.tr('Use light theme')
            iconName: 'torch-off'
            visible: gallery.theme.name == 'Ubuntu.Components.Themes.SuruDark'
            onTriggered: gallery.theme.name = 'Ubuntu.Components.Themes.Ambiance'
        },
        Action {
            text: i18n.tr('About')
            iconName: "info"
            onTriggered: mainPage.pageStack.addPageToCurrentColumn(mainPage, Qt.resolvedUrl("About.qml"))
        }
    ]

    onActiveChanged: {
        if (layout.columns < 2) {
            widgetList.currentIndex = -1;
        }
        if (active) {
            widgetList.openPage();
        }
    }

    Rectangle {
        color: Qt.rgba(0.0, 0.0, 0.0, 0.01)
        anchors.fill: parent

        UbuntuListView {
            id: widgetList
            objectName: "widgetList"
            anchors.fill: parent
            model: WidgetsModel {}
            currentIndex: -1

            onCurrentIndexChanged: openPage()

            function openPage() {
                if (!mainPage.active || currentIndex < 0) return;
                var modelData = model.get(currentIndex);
                var source = Qt.resolvedUrl(modelData.source);
                mainPage.pageStack.addPageToNextColumn(mainPage, source, {title: modelData.label});
            }

            delegate: ListItem {
                objectName: model.objectName
                contentItem {
                    anchors.leftMargin: units.gu(2)
                    anchors.rightMargin: units.gu(2)
                }
                enabled: source != ""
                // Used by Autopilot
                property string text: label
                onClicked: widgetList.currentIndex = index
                Label {
                    id: labelItem
                    anchors {
                        fill: parent
                        rightMargin: units.gu(4)
                    }
                    text: label
                    verticalAlignment: Text.AlignVCenter
                }
                Icon {
                    name: "next"
                    width: units.gu(2)
                    height: units.gu(2)
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                }
            }
            highlight: Rectangle {
                color: theme.palette.selected.background
            }
            highlightMoveDuration: 0
        }
    }
}
