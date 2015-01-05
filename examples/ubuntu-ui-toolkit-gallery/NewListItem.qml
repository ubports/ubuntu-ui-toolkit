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

import QtQuick 2.2
import Ubuntu.Components 1.2

Template {
    objectName: "listItemsTemplate"

    // FIXME TIM: Do something with the sections.
    TemplateSection {
        className: "ListItem"

        Rectangle {
            color: "white"//#CCCCCC"
            width: parent.width
            height: units.gu(36)

            Flickable {
                anchors.fill: parent

                Column {
                    width: parent.width
                    height: childrenRect.height

                    ListItem {
                        Label {
//                            anchors {
//                                left: parent.left
//                                leftMargin: units.gu(2)
//                                verticalCenter: parent.verticalCenter
//                            }
                            text: i18n.tr("Basic")
                        }
                    }
                    ListItem {
                        color: UbuntuColors.blue
                        Label {
//                            anchors {
//                                left: parent.left
//                                leftMargin: units.gu(2)
//                                verticalCenter: parent.verticalCenter
//                            }
                            text: i18n.tr("Colored")
                        }
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("No divider")
                        }
                        divider.visible: false
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("Colored divider")
                        }
                        divider {
                            colorFrom: UbuntuColors.red
                            colorTo: UbuntuColors.green
                        }
                    }
                    ListItem {
                        Label {
                            text: i18n.tr("Highlight color")
                        }
                        highlightColor: UbuntuColors.purple
                        onClicked: print(i18n.tr("Highlighting list item"))
                    }
                }
            }
        }
    }
}
