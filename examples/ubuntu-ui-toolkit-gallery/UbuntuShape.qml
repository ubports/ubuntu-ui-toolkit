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
            title: i18n.tr("Aspect")
            titleWidth: units.gu(8)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_aspect_inset"
                backgroundColor: UbuntuColors.orange
                radius: "medium"
                aspect: UbuntuShape.Inset
                Label {
                    anchors.centerIn: parent
                    text: "Inset"
                    fontSize: "x-small"
                    color: theme.palette.normal.foregroundText
                }
            }

            UbuntuShape {
                objectName: "ubuntushape_aspect_dropshadow"
                backgroundColor: UbuntuColors.orange
                radius: "medium"
                aspect: UbuntuShape.DropShadow
                Label {
                    anchors.centerIn: parent
                    text: "DropShadow"
                    fontSize: "xx-small"
                    color: theme.palette.normal.foregroundText
                }
            }

            UbuntuShape {
                objectName: "ubuntushape_aspect_flat"
                backgroundColor: UbuntuColors.orange
                radius: "medium"
                aspect: UbuntuShape.Flat
                Label {
                    anchors.centerIn: parent
                    text: "Flat"
                    fontSize: "xx-small"
                    color: theme.palette.normal.foregroundText
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Radius")
            titleWidth: units.gu(8)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_radius_small"
                backgroundColor: theme.palette.normal.foreground
                radius: "small"
                Label {
                    anchors.centerIn: parent
                    text: "small"
                    fontSize: "xx-small"
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
                    fontSize: "xx-small"
                    color: theme.palette.normal.foregroundText
                }
            }

            UbuntuShape {
                objectName: "ubuntushape_radius_medium"
                backgroundColor: theme.palette.normal.foreground
                radius: "large"
                Label {
                    anchors.centerIn: parent
                    text: "large"
                    fontSize: "xx-small"
                    color: theme.palette.normal.foregroundText
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Image")
            titleWidth: units.gu(8)
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
            title: i18n.tr("Gradient")
            titleWidth: units.gu(8)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_verticalgradient"
                backgroundColor: UbuntuColors.lightAubergine
                secondaryBackgroundColor: Qt.rgba(
                    UbuntuColors.lightAubergine.r, UbuntuColors.lightAubergine.g,
                    UbuntuColors.lightAubergine.b, 0.25)
                backgroundMode: UbuntuShape.VerticalGradient
            }
        }
    }
}
