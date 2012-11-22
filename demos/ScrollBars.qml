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
import Ubuntu.Components.ListItems 0.1

Template {
    title: i18n.tr("Scrollbar")
    id: scrollbars
    LayoutMirroring.childrenInherit: true

    Column {
        spacing: units.gu(4)

        TemplateRow {
            title: i18n.tr("Flickable")
            height: scrollable.height

            Item {
                id: scrollable
                width: units.gu(37.5)
                height: units.gu(23)
                Flickable {
                    id: flickable
                    anchors.fill: parent
                    contentHeight: image.sourceSize.height
                    contentWidth: image.sourceSize.width
                    clip: true
                    Image {
                        id: image
                        source: Qt.resolvedUrl("demo_image.jpg")
                    }
                }
                Scrollbar {
                    flickableItem: flickable
                    align: Qt.AlignTrailing
                }
                Scrollbar {
                    flickableItem: flickable
                    align: Qt.AlignBottom
                }
            }
        }

        TemplateRow {
            title: i18n.tr("ListView")
            height: scrollable2.height

            Rectangle {
                id: scrollable2
                width: units.gu(37.5)
                height: units.gu(23)
                color: "transparent"
                border.width: units.dp(1)
                border.color: "#eeeeee"
                radius: units.dp(5)

                ListView {
                    id: listView
                    clip: true
                    anchors.fill: parent
                    model: listModel

                    section.property: "title"
                    section.criteria: ViewSection.FirstCharacter
                    section.delegate: Header {
                        text: "Section - " + section
                    }

                    delegate: Standard {
                        text: "Data - " + label
                    }
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
                    align: Qt.AlignTrailing
                }
            }
        }
    }
}
