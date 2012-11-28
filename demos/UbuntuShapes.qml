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
    title: i18n.tr("Ubuntu Shape")

    Column {
        spacing: units.gu(4)


        TemplateRow {
            title: i18n.tr("Radius")
            height: units.gu(8)

            UbuntuShape {
                color: "#c94212"
                radius: "small"

                TextCustom {
                    anchors.centerIn: parent
                    text: "small"
                    fontSize: "x-small"
                    color: "#f1f1f1"
                }
            }

            UbuntuShape {
                color: "#c94212"
                radius: "medium"

                TextCustom {
                    anchors.centerIn: parent
                    text: "medium"
                    fontSize: "x-small"
                    color: "#f1f1f1"
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Color")
            height: units.gu(8)

            UbuntuShape {
                color: "#5da357"
            }

            UbuntuShape {
                color: "lightblue"
            }

            UbuntuShape {
                color: "purple"
            }

            UbuntuShape {
                color: "darkgrey"
            }
        }

        TemplateRow {
            title: i18n.tr("Image")
            height: units.gu(8)

            MouseArea {
                width:  units.gu(14)
                height: units.gu(8)
                Image {
                    anchors.fill: parent
                    id: imageStretch
                    source: "map_icon.png"
                    visible: parent.pressed
                }
                UbuntuShape {
                    image: imageStretch
                    anchors.fill: parent
                    visible: !parent.pressed
                }
            }

            MouseArea {
                width: units.gu(14)
                height: units.gu(8)
                Image {
                    id: imagePreserveAspectCropTopLeft
                    anchors.fill: parent
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                    horizontalAlignment: Image.AlignLeft
                    verticalAlignment: Image.AlignTop
                    visible: parent.pressed
                }
                UbuntuShape {
                    image: imagePreserveAspectCropTopLeft
                    anchors.fill: parent
                    visible: !parent.pressed
                }
            }

            MouseArea {
                width: units.gu(14)
                height: units.gu(8)
                Image {
                    id: imagePreserveAspectCropBottomRight
                    anchors.fill: parent
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                    horizontalAlignment: Image.AlignRight
                    verticalAlignment: Image.AlignBottom
                    visible: parent.pressed
                }
                UbuntuShape {
                    image: imagePreserveAspectCropBottomRight
                    anchors.fill: parent
                    visible: !parent.pressed
                }
            }

            MouseArea {
                width: units.gu(14)
                height: units.gu(8)
                Image {
                    id: imagePreserveAspectCropCenter
                    anchors.fill: parent
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    visible: parent.pressed
                }
                UbuntuShape {
                    image: imagePreserveAspectCropCenter
                    anchors.fill: parent
                    visible: !parent.pressed
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Resizing")
            height: units.gu(20)

            UbuntuShape {
                color: "#c94212"
            }

            UbuntuShape {
                color: "#c94212"
                width: units.gu(10)
                height: units.gu(4)
            }

            UbuntuShape {
                color: "#c94212"
                width: units.gu(10)
                height: units.gu(20)
            }
        }
    }
}
