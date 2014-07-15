/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.1
import Ubuntu.Layouts 1.0

MainView {
    id: main
    objectName: "mainView"
    applicationName: "test-app"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true // TODO: support wide aspect ratios, esp. on prepareGamePage

    width: units.gu(40)
    height: units.gu(75)

    function phoneLayout() {
        width = units.gu(50);
    }
    function tabletPortraitLayout() {
        width = units.gu(70);
    }
    function tabletLandscapeLayout() {
        width = units.gu(100);
    }

    PageStack {
        Component.onCompleted: push(testPage)

        Page {
            title: i18n.tr("Conditional Layouts with PageStack")
            id: testPage
            visible: false

            Layouts {
                id: layouts
                objectName: "mainLayout"
                width: parent.width
                height: parent.height
                layouts: [
                    ConditionalLayout {
                        name: "tabletPortrait"
                        when: main.width > units.gu(50)
                        Layouts {
                            anchors.fill: parent
                            objectName: "nestedLayout"
                            layouts: [
                                ConditionalLayout {
                                    name: "tabletLandscape"
                                    when: main.width > units.gu(70)
                                    Row {
                                        objectName: "tabletLandscape"
                                        anchors.fill: parent
                                        Rectangle {
                                            height: parent.height
                                            color: "white"
                                            width: parent.width / 5
                                        }
                                        Flow {
                                            spacing: units.gu(2)
                                            ItemLayout {
                                                objectName: "nestedMagentaContainer"
                                                item: "mainMagenta"
                                                width: units.gu(20)
                                                height: width
                                            }
                                            ItemLayout {
                                                objectName: "nestedGreenContainer"
                                                item: "mainGreen"
                                                width: units.gu(60)
                                                height: width
                                            }
                                        }
                                    }
                                }
                            ]
                            // Default layout, but can have ItemLayouts
                            // as those will belong to the previous Layouts
                            // and attaching Layouts.item we can layout it further
                            Flow {
                                objectName: "tabletPortrait"
                                anchors.fill: parent
                                ItemLayout {
                                    objectName: "greenContainer"
                                    item: "mainGreen"
                                    width: units.gu(40)
                                    height: width
                                    Layouts.item: item
                                }
                                ItemLayout {
                                    objectName: "magentaContainer"
                                    item: "mainMagenta"
                                    width: units.gu(60)
                                    height: width
                                    Layouts.item: item
                                }
                            }
                        }
                    }
                ]

                // default layout for mainLayout
                Flow {
                    objectName: "mainDefaultLayout"
                    anchors.fill: parent
                    Rectangle {
                        objectName: "mainRed"
                        width: units.gu(15)
                        height: width
                        color: "red"
                        Layouts.item: "mainRed"
                    }
                    Rectangle {
                        objectName: "mainGreen"
                        width: units.gu(15)
                        height: width
                        color: "green"
                        Layouts.item: "mainGreen"
                    }
                    Rectangle {
                        objectName: "mainBlue"
                        width: units.gu(15)
                        height: width
                        color: "blue"
                        Layouts.item: "mainBlue"
                    }
                    Rectangle {
                        objectName: "mainMagenta"
                        width: units.gu(15)
                        height: width
                        color: "magenta"
                        Layouts.item: "mainMagenta"
                    }
                    Rectangle {
                        objectName: "mainHidden"
                        width: units.gu(15)
                        height: width
                        color: "yellow"
                        visible: false
                    }
                }
            }
        }
    }
}
