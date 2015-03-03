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

Template {
    objectName: "ubuntuShapesTemplate"

    TemplateSection {
        title: "Ubuntu Shape"
        className: "UbuntuShape"

        TemplateRow {
            title: i18n.tr("Color")
            titleWidth: units.gu(6)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_color_hex"
                color: UbuntuColors.orange
            }

            UbuntuShape {
                objectName: "ubuntushape_color_lightaubergine"
                color: UbuntuColors.lightAubergine
            }

            UbuntuShape {
                objectName: "ubuntushape_color_darkgray"
                color: UbuntuColors.warmGrey
            }
        }

        TemplateRow {
            title: i18n.tr("Image")
            titleWidth: units.gu(6)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_image"

                image: Image {
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Radius")
            titleWidth: units.gu(6)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_radius_small"

                color: Theme.palette.normal.foreground
                radius: "small"

                Label {
                    anchors.centerIn: parent
                    text: "small"
                    fontSize: "x-small"
                    color: Theme.palette.normal.foregroundText
                }
            }

            UbuntuShape {
                objectName: "ubuntushape_radius_medium"
                color: Theme.palette.normal.foreground
                radius: "medium"

                Label {
                    anchors.centerIn: parent
                    text: "medium"
                    fontSize: "x-small"
                    color: Theme.palette.normal.foregroundText
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Sizes")
            titleWidth: units.gu(6)
            height: units.gu(20)

            UbuntuShape {
                objectName: "ubuntushape_sizes_15_6"
                color: Theme.palette.normal.foreground
                width: units.gu(15)
                height: units.gu(6)
                anchors.verticalCenter: parent.verticalCenter
            }

            UbuntuShape {
                objectName: "ubuntushape_sizes_10_14"
                color: Theme.palette.normal.foreground
                width: units.gu(10)
                height: units.gu(14)
            }
        }
    }
}
