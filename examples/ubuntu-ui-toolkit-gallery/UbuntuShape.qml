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
    objectName: "ubuntuShapesTemplate"

    TemplateSection {
        title: "Ubuntu Shape"
        className: "UbuntuShape"

        TemplateRow {
            title: i18n.tr("Color")
            titleWidth: units.gu(6)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_backgroundcolor_orange"
                backgroundColor: UbuntuColors.orange
            }

            UbuntuShape {
                objectName: "ubuntushape_verticalgradient"
                backgroundColor: UbuntuColors.lightAubergine
                secondaryBackgroundColor: Qt.rgba(
                    UbuntuColors.lightAubergine.r, UbuntuColors.lightAubergine.g,
                    UbuntuColors.lightAubergine.b, 0.25)
                backgroundMode: UbuntuShape.VerticalGradient
            }
        }

        TemplateRow {
            title: i18n.tr("Image")
            titleWidth: units.gu(6)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_preserveaspectcrop"
                source: Image { source: "map_icon.png" }
                sourceFillMode: UbuntuShape.PreserveAspectCrop
            }

            UbuntuShape {
                objectName: "ubuntushape_pad"
                backgroundColor: UbuntuColors.warmGrey
                source: Image { source: "images.png" }
                sourceFillMode: UbuntuShape.Pad
            }
        }

        TemplateRow {
            title: i18n.tr("Radius")
            titleWidth: units.gu(6)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_radius_small"

                backgroundColor: theme.palette.normal.foreground
                radius: "small"

                Label {
                    anchors.centerIn: parent
                    text: "small"
                    fontSize: "x-small"
                    color: theme.palette.normal.foregroundText
                }
            }

            UbuntuShape {
                objectName: "ubuntushape_radius_medium"
                backgroundColor: theme.palette.normal.foreground
                radius: "medium"

                Label {
                    anchors.centerIn: parent
                    text: "medium"
                    fontSize: "x-small"
                    color: theme.palette.normal.foregroundText
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Sizes")
            titleWidth: units.gu(6)
            height: units.gu(20)

            UbuntuShape {
                objectName: "ubuntushape_sizes_15_6"
                backgroundColor: theme.palette.normal.foreground
                width: units.gu(15)
                height: units.gu(6)
                anchors.verticalCenter: parent.verticalCenter
            }

            UbuntuShape {
                objectName: "ubuntushape_sizes_10_14"
                backgroundColor: theme.palette.normal.foreground
                width: units.gu(10)
                height: units.gu(14)
            }
        }
    }
}
