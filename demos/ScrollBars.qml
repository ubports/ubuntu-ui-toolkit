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

Template {
    title: i18n.tr("Scrollbar")
    id: scrollbars

    Column {
        LayoutMirroring.childrenInherit: true
        //LayoutMirroring.enabled: true
        spacing: units.gu(4)

        Item {
            width: 300
            height: 220
            Flickable {
                id: flickable
                anchors.fill: parent
                contentHeight: image.sourceSize.height
                contentWidth: image.sourceSize.width
                clip: true
                Image {
                    id: image
                    source: Qt.resolvedUrl("rivieramaya3.jpg")
                }
            }
            Scrollbar {
                flickableItem: flickable
                align: "rear"
                orientation: Qt.Vertical
            }
            Scrollbar {
                flickableItem: flickable
                align: "bottom"
                orientation: Qt.Horizontal
            }
        }


        Item {
            width: 300
            height: 220
            Component {
                id: sectionDelegate
                Rectangle {
                    id: sectionHeader
                    width: 300
                    height: units.gu(4.2)
                    color: "#66AA77"
                    TextCustom {
                        anchors.fill: parent
                        text: section
                        font.bold: true
                    }
                }
            }

            Component {
                id: itemDelegate
                Rectangle {
                    id: itemFrame
                    width: 300
                    height: units.gu(6)
                    color: "#C4C5C4"
                    TextCustom {
                        anchors.fill: parent
                        text: label
                    }
                }
            }

            ListView {
                id: listView
                clip: true
                anchors.fill: parent
                model: listModel
                //section.property: "title"
                section.criteria: ViewSection.FirstCharacter
                //section.delegate: sectionDelegate
                delegate: itemDelegate
            }
            ListModel {
                id: listModel
                Component.onCompleted: {
                    for (var i = 0; i < 100; i++) {
                        listModel.append({"title": i, "label": i})
                    }
                }
            }

            Scrollbar {
                flickableItem: listView
                align: "rear"
                orientation: Qt.Vertical
            }
        }

    }
}
