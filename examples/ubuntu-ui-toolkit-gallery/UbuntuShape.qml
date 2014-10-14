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

import QtQuick 2.2
import Ubuntu.Components 1.1

Template {
    objectName: "ubuntuShapesTemplate"

    TemplateSection {
        title: "Ubuntu Shape"
        className: "UbuntuShape"

        TemplateRow {
            title: i18n.tr("Color")
            titleWidth: units.gu(7)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_color_orange"
                backgroundColor: UbuntuColors.orange
            }

            UbuntuShape {
                objectName: "ubuntushape_color_lightAubergine"
                backgroundColor: UbuntuColors.lightAubergine
            }

            UbuntuShape {
                objectName: "ubuntushape_color_gradient"
                backgroundColor: UbuntuColors.green
                secondaryBackgroundColor: UbuntuColors.darkGrey
                backgroundMode: UbuntuShape.VerticalGradient
            }
        }

        TemplateRow {
            title: i18n.tr("Image")
            titleWidth: units.gu(7)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_image"
                image: Image {
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                }
            }

            UbuntuShape {
                objectName: "ubuntushape_image_background"
                backgroundColor: UbuntuColors.orange
                image: Image {
                    source: "images.png"
                    fillMode: Image.PreserveAspectFit
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Radius")
            titleWidth: units.gu(7)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_radius_small"

                backgroundColor: Theme.palette.normal.foreground
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
                backgroundColor: Theme.palette.normal.foreground
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
            title: i18n.tr("Overlay")
            titleWidth: units.gu(7)
            height: units.gu(8)

            UbuntuShape {
                objectName: "ubuntushape_image_overlay"
                image: Image {
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                }
                overlayColor: Qt.rgba(0.0, 0.0, 0.0, 0.5)
                overlayGeometry: Qt.rect(0.0, 0.5, 1.0, 0.5)
            }

            UbuntuShape {
                objectName: "ubuntushape_color_overlay"
                radius: "medium"
                backgroundColor: UbuntuColors.lightAubergine
                overlayColor: Qt.rgba(1.0, 1.0, 1.0, 0.25)
                overlayGeometry: Qt.rect(0.25, 0.25, 0.5, 0.5)
            }
        }

        TemplateRow {
            title: i18n.tr("Sizes")
            titleWidth: units.gu(7)
            height: units.gu(20)

            UbuntuShape {
                objectName: "ubuntushape_sizes_15_6"
                backgroundColor: Theme.palette.normal.foreground
                width: units.gu(15)
                height: units.gu(6)
                anchors.verticalCenter: parent.verticalCenter
            }

            UbuntuShape {
                objectName: "ubuntushape_sizes_10_14"
                backgroundColor: Theme.palette.normal.foreground
                width: units.gu(10)
                height: units.gu(14)
            }
        }
    }
}
