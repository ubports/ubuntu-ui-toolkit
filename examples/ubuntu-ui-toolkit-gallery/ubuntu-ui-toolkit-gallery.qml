/*
 * Copyright 2013 Canonical Ltd.
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

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    state: width >= units.gu(80) ? "wide" : "narrow"
    states: [
        State {
            name: "narrow"
            StateChangeScript {
                script: {
                    pageStack.push(mainPage);
                    if (selectedWidget) {
                        pageStack.push(contentPage);
                    }
                }
            }
            PropertyChanges {
                target: mainPage
                flickable: widgetList
            }
            PropertyChanges {
                target: contentPage
                flickable: contentLoader.item ? contentLoader.item.flickable : null
            }
        },
        State {
            name: "wide"
            StateChangeScript {
                script: {
                    pageStack.clear();

                    /* When pushing Pages into a PageStack they are reparented
                       to internally created PageWrappers. This undoes it as to
                       allow us to anchor the Pages freely again.
                    */
                    mainPage.parent = gallery;
                    contentPage.parent = gallery;
                }
            }
            PropertyChanges {
                target: mainPage
                width: units.gu(40)
                clip: true
            }
            AnchorChanges {
                target: mainPage
                anchors.right: undefined
            }
            PropertyChanges {
                target: contentPage
                clip: true
            }
            AnchorChanges {
                target: contentPage
                anchors.left: mainPage.right
            }
        }
    ]

    property var selectedWidget: null

    Page {
        id: mainPage
        active: selectedWidget == null

        title: "Ubuntu UI Toolkit"
        /* Page internally sets the topMargin of its flickable to account for
           the height of the header. Undo it when unsetting the flickable.
        */
        onFlickableChanged: if (!flickable) widgetList.topMargin = 0;

        head.actions: [
            Action {
                text: i18n.tr('Use dark theme')
                iconName: 'torch-on'
                visible: theme.name == 'Ubuntu.Components.Themes.Ambiance'
                onTriggered: theme.name = 'Ubuntu.Components.Themes.SuruDark'
            },
            Action {
                text: i18n.tr('Use light theme')
                iconName: 'torch-off'
                visible: theme.name == 'Ubuntu.Components.Themes.SuruDark'
                onTriggered: theme.name = 'Ubuntu.Components.Themes.Ambiance'
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
                delegate: ListItem.Standard {
                    text: model.label
                    objectName: model.objectName
                    enabled: model.source != ""
                    progression: true
                    selected: enabled && selectedWidget == model
                    onClicked: {
                        selectedWidget = model;
                        if (gallery.state == "narrow") {
                            pageStack.push(contentPage);
                        }
                    }
                }
            }
        }
    }

    Page {
        id: contentPage
        active: selectedWidget != null
        title: selectedWidget ? selectedWidget.label : ""
        /* Page internally sets the topMargin of its flickable to account for
           the height of the header. Undo it when unsetting the flickable.
        */
        onFlickableChanged: if (!flickable && contentLoader.item) contentLoader.item.flickable.topMargin = 0;
        onActiveChanged: if (gallery.state == "narrow" && !active) {
                             selectedWidget = null;
                         }

        Loader {
            id: contentLoader
            objectName: "contentLoader"
            anchors.fill: parent
            source: selectedWidget ? selectedWidget.source : ""
        }
    }

    PageStack {
        id: pageStack
    }

    WidgetsModel {
        id: widgetsModel
    }
}
