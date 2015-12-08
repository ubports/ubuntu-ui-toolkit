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


Template {
    objectName: "scrollbarsTemplate"

    TemplateSection {
        title: "Scrollbars"
        className: "Scrollbar"
        Item {
            id: container1
            width: units.gu(30)
            height: units.gu(40)
            Flickable {
                id: flickable1
                anchors.fill: parent
                contentHeight: rect1.height
                contentWidth: rect1.width
                clip: true
                Rectangle {
                    id: rect1
                    color: "yellow"
                    width: units.gu(60)
                    height: units.gu(50)
                }
            }
            Scrollbar {
                id: scrollbar1
                align: Qt.AlignLeading
                flickableItem: flickable1
                __buddyScrollbar: scrollbar2
            }
            Scrollbar {
                id: scrollbar2
                align: Qt.AlignTop
                flickableItem: flickable1
                __buddyScrollbar: scrollbar1
            }
        }

        Item {
            id: container2
            width: units.gu(30)
            height: units.gu(40)

            Flickable {
                id: flickable2
                anchors.fill: parent
                contentHeight: rect2.height
                contentWidth: rect2.width
                clip: true
                Rectangle {
                    id: rect2
                    color: "blue"
                    width: units.gu(60)
                    height: units.gu(50)
                }
            }
            Scrollbar {
                id: scrollbar3
                align: Qt.AlignTrailing
                flickableItem: flickable2
                __buddyScrollbar: scrollbar4
            }
            Scrollbar {
                id: scrollbar4
                align: Qt.AlignBottom
                flickableItem: flickable2
                __buddyScrollbar: scrollbar3
            }
        }

        Item {
            id: container3
            width: units.gu(30)
            height: units.gu(40)

            Flickable {
                id: flickable3
                anchors.fill: parent
                contentHeight: rect3.height
                contentWidth: rect3.width
                clip: true
                Rectangle {
                    id: rect3
                    color: "green"
                    width: units.gu(60)
                    height: units.gu(50)
                }
            }
            Scrollbar {
                id: scrollbar5
                align: Qt.AlignTrailing
                flickableItem: flickable3
                __buddyScrollbar: scrollbar6
                //__alwaysOnScrollbars: true
            }
            Scrollbar {
                id: scrollbar6
                align: Qt.AlignBottom
                flickableItem: flickable3
                __buddyScrollbar: scrollbar5
                //__alwaysOnScrollbars: true
            }
        }
        Rectangle {
            width: units.gu(30)
            height: units.gu(2)
            color: "black"
        }


        Item {
            id: container4
            width: units.gu(30)
            height: units.gu(40)
            ScrollView {
                id: view
                anchors.fill: parent
                Image {
                    source: "http://files.explosm.net/comics/Rob/sbTotal.png"
                }
            }
        }

        Item {
            id: container5

            width: units.gu(60)
            height: units.gu(40)

            ScrollView {
                anchors.fill: parent
                //Image {
                //    source: "http://mapdesign.icaci.org/wp-content/uploads/2014/07/MapCarte197_tokyo_large.png"
                //}

                //quick way to test the "hinting" behaviour when the contentItem size changes
                Button {
                    text :"Trigger hint"
                    onClicked: view.flickableItem.contentWidth+=10
                }
            }
        }
    }
}
