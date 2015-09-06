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
import Ubuntu.Components.ListItems 1.3 as ListItem

MainView {
    id: gallery
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the .desktop filename
    applicationName: "ubuntu-ui-toolkit-gallery"

    width: units.gu(120)
    height: units.gu(75)

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    AdaptivePageLayout {
        id: layout
        anchors.fill: parent
        primaryPage: mainPage

        layouts: [
            PageColumnsLayout {
                when: layout.width > units.gu(80)
                PageColumn {
                    minimumWidth: units.gu(30)
                    maximumWidth: units.gu(50)
                    preferredWidth: units.gu(40)
                }
                PageColumn {
                    fillWidth: true
                }
            },
            // configure single column mode so we can only size it to minimum 20 GU
            PageColumnsLayout {
                when: true
                PageColumn {
                    minimumWidth: units.gu(20)
                    fillWidth: true
                }
            }
        ]

        Page {
            id: mainPage
            title: "Ubuntu UI Toolkit"

            head.actions: [
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
                }
            ]

            Rectangle {
                color: Qt.rgba(0.0, 0.0, 0.0, 0.01)
                anchors.fill: parent

                ListView {
                    id: widgetList
                    objectName: "widgetList"
                    anchors.fill: parent
                    model: widgetsModel
                    currentIndex: -1
                    delegate: ListItem.Standard {
                        text: model.label
                        objectName: model.objectName
                        enabled: model.source != ""
                        progression: true
                        selected: index === widgetList.currentIndex
                        onClicked: {
                            var source = Qt.resolvedUrl(model.source);
                            layout.addPageToNextColumn(mainPage, source, {title: model.label});

                            widgetList.currentIndex = index;
                        }
                    }
                }
            }
        }
    }

    WidgetsModel {
        id: widgetsModel
    }
}
