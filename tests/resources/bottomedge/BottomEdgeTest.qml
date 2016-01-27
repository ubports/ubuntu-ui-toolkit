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


MainView {
    id: mainWindow

    width: units.gu(40)
    height: units.gu(70)

    AdaptivePageLayout {
        anchors.fill: parent
        primaryPage: page0
    }

    Page {
        id: page0

        header: PageHeader {
            title: "MainPage"
        }

        BottomEdge {
            id: bottomEdge

            anchors.fill: parent
            hint {
                flickable: page0.flickable
                action: Action {
                    iconName: "contact-new"
                    onTriggered: bottomEdge.commit()
                    shortcut: "ctrl+n"
                    enabled: bottomEdge.enabled
                }
            }

            contentComponent: editorPageBottomEdge

            Component {
                id: editorPageBottomEdge

                Page {
                    height: bottomEdge.height
                    width: bottomEdge.width - units.gu(15)
                    anchors.left: undefined
                    anchors.right: undefined

                    header: PageHeader {
                        title: "bottom edge page"
                        flickable: flicker
                        leadingActionBar.actions: Action {
                            iconName: "info"
                            onTriggered: bottomEdge.collapse()
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: "green"
                    }

                    Flickable {
                        id: flicker
                        anchors.fill: parent
                        Column {
                            id: view
                            anchors.fill: parent
                            Repeater {
                                model: 100

                                TextField {
                                    height: units.gu(10)
                                    width: parent.width
                                    //border.color: "black"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: QuickUtils.mouseAttached = true
}
