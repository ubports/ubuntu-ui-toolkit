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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    id: gallery
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the .desktop filename
    applicationName: "Gallery"


    width: units.gu(120)
    height: units.gu(75)

    property bool wideAspect: width >= units.gu(80)
    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true

    state: wideAspect ? "wide" : ""
    states: [
        State {
            name: "wide"
            PropertyChanges {
                target: pageStack
                width: units.gu(40)
                anchors {
                    fill: null
                    top: parent.top
                    bottom: parent.bottom
                }
            }
            PropertyChanges {
                target: contentPage
                x: pageStack.width
                width: pageStack.parent.width - x
                anchors {
                    left: undefined
                    right: undefined
                    bottom: undefined
                }
                clip: true
                visible: true
            }
        }
    ]

    PageStack {
        id: pageStack
        Component.onCompleted: push(mainPage)

        Page {
            id: mainPage
            title: "Ubuntu UI Toolkit"
            visible: false
            flickable: widgetList

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
                        enabled: model.source != ""
                        progression: true
                        selected: enabled && contentPage.source == Qt.resolvedUrl(model.source)
                        onClicked: {
                            contentPage.title = model.label;
                            contentPage.source = model.source;
                            if (!wideAspect) {
                                pageStack.push(contentPage);
                            }
                        }
                    }
                }
            }
        }

        Page {
            id: contentPage
            objectName: "contentPage"
            visible: false
            property alias source: contentLoader.source
            onActiveChanged: if (!active) source = ""
            ToolbarItems{ id: defTools}
            tools: contentLoader.item && contentLoader.item.tools ? contentLoader.item.tools : defTools
            flickable: contentLoader.item && !wideAspect ? contentLoader.item.flickable : null

            Loader {
                id: contentLoader
                objectName: "contentLoader"
                anchors.fill: parent
            }
        }
    }

    WidgetsModel {
        id: widgetsModel
    }
}
